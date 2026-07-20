#include "m57.h"

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