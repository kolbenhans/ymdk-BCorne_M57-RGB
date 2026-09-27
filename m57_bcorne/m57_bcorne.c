#include "m57_bcorne.h"

#ifdef RGB_MATRIX_ENABLE

led_config_t g_led_config = {
    {
        // Matrix rows 0-4 = left half, rows 5-9 = right half.
        // Chain order left:
        // top(0-5) > Q row(6-11) > PgUp(12) > home row(13-18)
        // > PgDn(19) > Z row(20-25) > thumbs(26-28)
        //
        // Notes:
        // - PgDn at matrix [2,6] has LED 19.
        // - Mute at matrix [3,6] has no LED.
        // - Right half is mapped sequentially from LED 29 to 57.

        { 0,      1,      2,      3,      4,      5,      NO_LED },
        { 6,      7,      8,      9,      10,     11,     12     },
        { 13,     14,     15,     16,     17,     18,     19     },
        { 20,     21,     22,     23,     24,     25,     NO_LED },
        { NO_LED, NO_LED, NO_LED, 26,     27,     28,     NO_LED },

        { NO_LED, 29,     30,     31,     32,     33,     34     },
        { 35,     36,     37,     38,     39,     40,     41     },
        { 42,     43,     44,     45,     46,     47,     48     },
        { NO_LED, 49,     50,     51,     52,     53,     54     },
        { NO_LED, 55,     56,     57,     NO_LED, NO_LED, NO_LED }
    },
    {
        // LED index to physical position.

        { 0,   12 }, { 16,  12 }, { 32,  12 }, { 48,  12 }, { 64,  12 }, { 80,  12 },
        { 0,   25 }, { 16,  25 }, { 32,  25 }, { 48,  25 }, { 64,  25 }, { 80,  25 }, { 96,  25 },
        { 0,   38 }, { 16,  38 }, { 32,  38 }, { 48,  38 }, { 64,  38 }, { 80,  38 }, { 96,  38 },
        { 0,   51 }, { 16,  51 }, { 32,  51 }, { 48,  51 }, { 64,  51 }, { 80,  51 },
                                  { 32,  63 }, { 48,  63 }, { 64,  63 },

                                  { 128, 12 }, { 144, 12 }, { 160, 12 }, { 178, 12 }, { 194, 12 }, { 210, 12 },
        { 112, 25 }, { 128, 25 }, { 144, 25 }, { 160, 25 }, { 178, 25 }, { 194, 25 }, { 210, 25 },
        { 112, 38 }, { 128, 38 }, { 144, 38 }, { 160, 38 }, { 178, 38 }, { 194, 38 }, { 210, 38 },
                                  { 128, 51 }, { 144, 51 }, { 160, 51 }, { 178, 51 }, { 194, 51 }, { 210, 51 },
                    { 112, 63 }, { 128, 63 }, { 144, 63 }
    },
    {
        // LED index to flag.
        // 4 = LED_FLAG_KEYLIGHT

        4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4,
        4, 4, 4,

        4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4, 4, 4,
        4, 4, 4
    }
};

#endif

// PlumBL bootloader soft-entry:
// Write magic 0xc220b134 to 0x2000fc00, then reset.
//
// Spec from upstream:
// https://github.com/HaiMianBBao/PlumBL
//
// This overrides the weak empty stub from:
// platforms/chibios/bootloaders/custom.c
void bootloader_jump(void) {
    *(volatile uint32_t *)0x2000FC00UL = 0xC220B134UL;
    NVIC_SystemReset();
}

// Cold-boot USB self-heal. On a true cold power-up the host can see D+ raised
// before our USB stack answers control transfers; its enumeration attempts
// fail with -71 errors and the port backs off (many seconds to a working
// keyboard, or dead until a physical replug behind some hubs). A
// device-side disconnect/reconnect once we're fully initialized is equivalent
// to that replug. Bounded: at most 3 retries, 3s apart, only until the
// device reports CONFIGURED.
#include "usb_device_state.h"
#include "usb_main.h"
#include "split_util.h"

static void retry_master_enumeration(void) {
    static uint32_t next_check = 1000; // first check ~1s after the main loop starts
    static uint8_t  attempts   = 0;

    if (attempts >= 3) return;
    if (timer_read32() < next_check) return;

    if (usb_device_state_get_configure_state() == USB_DEVICE_STATE_CONFIGURED) {
        attempts = 3; // enumerated - done for this power-up
        return;
    }
    attempts++;
    next_check = timer_read32() + 3000;
    restart_usb_driver(&USB_DRIVER);
}

// Master/slave self-heal. SPLIT_USB_TIMEOUT (config.h) is kept short so a
// half without a cable isn't stuck for long — but that means a half *with*
// a cable can just as easily lose the same short race on a slow host (e.g.
// disk-encryption prompt delaying USB bring-up) and wrongly settle on
// slave, usb_disconnect()'d, typing nothing until replugged. Since it's
// still sitting on the cable, its own USB port will come alive once the
// host really does bring USB up — so periodically retry enumeration here
// too, and once it succeeds, reset: split_pre_init() re-runs from scratch on
// a fully-up USB port and this time correctly picks master immediately.
// Bounded to ~60s of retries; a half with genuinely no cable never sees
// CONFIGURED and just stays slave, as it should.
static void retry_master_promotion(void) {
    static uint32_t next_check = 3000;
    static uint8_t  attempts   = 0;

    if (attempts >= 20) return; // ~60s of retries, then stay slave for good
    if (timer_read32() < next_check) return;

    if (usb_device_state_get_configure_state() == USB_DEVICE_STATE_CONFIGURED) {
        NVIC_SystemReset(); // a host showed up after all - re-decide from scratch
    }
    attempts++;
    next_check = timer_read32() + 3000;
    restart_usb_driver(&USB_DRIVER);
}

void housekeeping_task_kb(void) {
    if (is_keyboard_master()) {
        retry_master_enumeration();
    } else {
        retry_master_promotion();
    }
}
