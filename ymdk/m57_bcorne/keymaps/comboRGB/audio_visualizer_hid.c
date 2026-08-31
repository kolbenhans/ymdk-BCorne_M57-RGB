// Keymap-level Raw HID glue for the kolbenhans/audio_visualizer module.
// Community modules can't hook raw_hid_receive_kb (not in QMK's
// module-hookable API list) — mode-switch dispatch stays here.
#include "audio_visualizer.h"

#ifdef RAW_ENABLE

// Mode-switch command sent by the Python viz tool (viz_hid.py). Returns
// true if consumed — also called directly by key_colors_hid.c when both
// modules are present (see key_colors/config.h, AUDIO_VISUALIZER_DISABLE_RAW_HID_HANDLER).
bool audio_visualizer_hid_handle_command(uint8_t *data, uint8_t length) {
    if (length < 2 || data[0] != 0x02) return false;

    switch (data[1]) {
        case 0xA3:
            audio_visualizer_trigger();
            rgb_matrix_mode_noeeprom(RGB_MATRIX_COMMUNITY_MODULE_audio_visualizer);
            return true;
    }
    return false;
}

#ifndef AUDIO_VISUALIZER_DISABLE_RAW_HID_HANDLER

// keypeek: optional, not an audio_visualizer dependency — auto-detected
// only if the keymap also lists srwi/keypeek_layer_notify in keymap.json.
#if __has_include("keypeek_layer_notify.h")
#    include "keypeek_layer_notify.h"
#    define AUDIO_VISUALIZER_HAS_KEYPEEK
#endif

void raw_hid_receive_kb(uint8_t *data, uint8_t length) {
#ifdef AUDIO_VISUALIZER_HAS_KEYPEEK
    if (keypeek_handle_command(data, length)) return;
#endif
    audio_visualizer_hid_handle_command(data, length);
}

#endif // AUDIO_VISUALIZER_DISABLE_RAW_HID_HANDLER

#endif // RAW_ENABLE
