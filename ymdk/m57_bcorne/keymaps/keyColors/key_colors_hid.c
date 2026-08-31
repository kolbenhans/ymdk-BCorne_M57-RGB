// Keymap-level Raw HID glue for the kolbenhans/key_colors module.
//
// Community modules cannot hook raw_hid_receive_kb (not in QMK's
// module-hookable API list) — the wire protocol (opcode numbers, 32-byte
// chunk framing) is a keymap/board concern, so it lives here and calls into
// the module's public API (key_colors_set_colors() etc.) instead of touching
// its internals directly.
#include QMK_KEYBOARD_H
#include "key_colors.h"

// keypeek: optional, not a key_colors dependency — auto-detected only if the
// keymap also lists srwi/keypeek_layer_notify in keymap.json.
#if __has_include("keypeek_layer_notify.h")
#    include "keypeek_layer_notify.h"
#    define KEY_COLORS_HAS_KEYPEEK
#endif

// audio_visualizer: same idea — auto-detected if the keymap also lists
// kolbenhans/audio_visualizer. We own raw_hid_receive_kb when both are
// present (audio_visualizer/config.h steps aside, see
// AUDIO_VISUALIZER_DISABLE_RAW_HID_HANDLER), just forward its commands to it.
#if __has_include("audio_visualizer.h")
#    define KEY_COLORS_HAS_AUDIO_VISUALIZER
extern bool audio_visualizer_hid_handle_command(uint8_t *data, uint8_t length);
#endif

#ifdef RAW_ENABLE

// WebGUI key-color chunk size — bounded by the 32-byte raw HID report minus
// the 5-byte header (family, subcmd, layer, led_offset, count).
#define KEY_COLOR_CHUNK_MAX 9

static void handle_get_key_colors(const uint8_t *req) {
    uint8_t layer      = req[2];
    uint8_t led_offset = req[3];
    uint8_t count      = req[4];
    if (count > KEY_COLOR_CHUNK_MAX) count = KEY_COLOR_CHUNK_MAX;

    uint8_t resp[32] = {0};
    resp[0] = 0x02;
    resp[1] = 0xA7;
    resp[2] = layer;
    resp[3] = led_offset;
    resp[4] = count;
    key_colors_get_colors(layer, led_offset, count, &resp[5]);
    host_raw_hid_send(resp, sizeof(resp));
}

static void handle_get_blink_colors(const uint8_t *req) {
    uint8_t layer      = req[2];
    uint8_t led_offset = req[3];
    uint8_t count      = req[4];
    if (count > KEY_COLOR_CHUNK_MAX) count = KEY_COLOR_CHUNK_MAX;

    uint8_t resp[32] = {0};
    resp[0] = 0x02;
    resp[1] = 0xAB;
    resp[2] = layer;
    resp[3] = led_offset;
    resp[4] = count;
    key_colors_get_blink_colors(layer, led_offset, count, &resp[5]);
    host_raw_hid_send(resp, sizeof(resp));
}

// Lock-flags chunk size — 1 byte per LED, same 32-byte report.
#define LOCK_FLAGS_CHUNK_MAX 27

static void handle_get_lock_flags(const uint8_t *req) {
    uint8_t layer      = req[2];
    uint8_t led_offset = req[3];
    uint8_t count      = req[4];
    if (count > LOCK_FLAGS_CHUNK_MAX) count = LOCK_FLAGS_CHUNK_MAX;

    uint8_t resp[32] = {0};
    resp[0] = 0x02;
    resp[1] = 0xA9;
    resp[2] = layer;
    resp[3] = led_offset;
    resp[4] = count;
    key_colors_get_lock_flags(layer, led_offset, count, &resp[5]);
    host_raw_hid_send(resp, sizeof(resp));
}

// Mode-switch (0xA4) and WebGUI key-color/lock-flag/blink-color commands
// (0xA5-0xAB), sent via WebHID.
void raw_hid_receive_kb(uint8_t *data, uint8_t length) {
#ifdef KEY_COLORS_HAS_KEYPEEK
    // keypeek claims its own subscribe/keepalive packets, ignores everything else
    if (keypeek_handle_command(data, length)) return;
#endif
#ifdef KEY_COLORS_HAS_AUDIO_VISUALIZER
    if (audio_visualizer_hid_handle_command(data, length)) return;
#endif

    if (length < 2 || data[0] != 0x02) return;

    switch (data[1]) {
        case 0xA4: // ACTIVATE_KEY_COLORS: switch into key_colors mode
            key_colors_on_mode_enter();
            rgb_matrix_mode_noeeprom(RGB_MATRIX_COMMUNITY_MODULE_key_colors);
            break;

        case 0xA5: { // SET_KEY_COLORS_CHUNK: layer, led_offset, count, (r,g,b)×count
            if (length < 5) return;
            uint8_t count = data[4];
            if (count > KEY_COLOR_CHUNK_MAX) count = KEY_COLOR_CHUNK_MAX;
            if (length < (uint16_t)5 + (uint16_t)count * 3) return;
            key_colors_set_colors(data[2], data[3], count, &data[5]);
            break;
        }

        case 0xA6: { // COMMIT_KEY_COLORS: persist current table to EEPROM
            key_colors_commit_colors();
            // Ack sent only after the flash write actually completes — the
            // host previously took "HID report sent" as "safe to reboot",
            // but that's just USB transfer done, not flash-write-done. A
            // reboot mid-write can corrupt the datablock (all colors lost).
            uint8_t resp[32] = {0};
            resp[0] = 0x02;
            resp[1] = 0xA6;
            host_raw_hid_send(resp, sizeof(resp));
            break;
        }

        case 0xA7: // GET_KEY_COLORS_CHUNK: layer, led_offset, count
            if (length < 5) return;
            handle_get_key_colors(data);
            break;

        case 0xA8: // SET_KEY_LOCK_FLAGS: layer, led, flags
            if (length < 5) return;
            key_colors_set_lock_flags(data[2], data[3], data[4]);
            break;

        case 0xA9: // GET_KEY_LOCK_FLAGS_CHUNK: layer, led_offset, count
            if (length < 5) return;
            handle_get_lock_flags(data);
            break;

        case 0xAA: { // SET_BLINK_COLORS_CHUNK: layer, led_offset, count, (r,g,b)×count
            if (length < 5) return;
            uint8_t count = data[4];
            if (count > KEY_COLOR_CHUNK_MAX) count = KEY_COLOR_CHUNK_MAX;
            if (length < (uint16_t)5 + (uint16_t)count * 3) return;
            key_colors_set_blink_colors(data[2], data[3], count, &data[5]);
            break;
        }

        case 0xAB: // GET_BLINK_COLORS_CHUNK: layer, led_offset, count
            if (length < 5) return;
            handle_get_blink_colors(data);
            break;
    }
}

#endif // RAW_ENABLE
