#pragma once

// Distinct from the `via` keymap's UID so Vial GUI/WebGUI don't confuse
// cached per-keyboard config between keymaps on the same hardware.
#define VIAL_KEYBOARD_UID {0x4B, 0xCB, 0x1D, 0x4D, 0x6E, 0xCA, 0xDA, 0x93}

#define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_CUSTOM_key_colors

// Board default is 10 — this keymap only needs 6, and fewer layers means
// less Vial dynamic-keymap EEPROM pressure too (see budget note below).
#undef DYNAMIC_KEYMAP_LAYER_COUNT
#define DYNAMIC_KEYMAP_LAYER_COUNT 6

// Split sync buffer large enough for 29 LEDs × 3 bytes RGB (87 bytes) —
// same size as the existing `via` keymap already needs for its own
// split-sync, so this board's WEAR_LEVELING/RPC setup already covers it.
#define RPC_M2S_BUFFER_SIZE 96

// WebGUI-assigned per-key/per-layer colors + blink colors + lock-state flags,
// persisted as a keyboard-level EEPROM datablock (key_colors.c:
// key_colors[6][58] + blink_colors[6][58] + key_lock_flags[6][58]).
// 6*58*3 (rgb) + 6*58*3 (blink rgb) + 6*58*1 (flags) = 1044+1044+348 = 2436 bytes.
#define EECONFIG_KB_DATA_SIZE 2436

// Board default (WEAR_LEVELING_LOGICAL_SIZE 4096 / BACKING_SIZE 8192) is
// already claimed by Vial's own dynamic keymap/combo/tapdance/macro data —
// the extra datablock above doesn't fit on top of that (hits a
// STATIC_ASSERT at compile time). Double both so everything coexists with
// headroom to spare.
#undef WEAR_LEVELING_LOGICAL_SIZE
#undef WEAR_LEVELING_BACKING_SIZE
#define WEAR_LEVELING_LOGICAL_SIZE 8192
#define WEAR_LEVELING_BACKING_SIZE (WEAR_LEVELING_LOGICAL_SIZE * 2)
#undef DYNAMIC_KEYMAP_EEPROM_MAX_ADDR
#define DYNAMIC_KEYMAP_EEPROM_MAX_ADDR (WEAR_LEVELING_LOGICAL_SIZE - 1)

// --- keypeek (srwi/keypeek module) ---
// Its own raw_hid_receive_kb calls raw_hid_receive_user(), which doesn't
// exist on vial-qmk. Disable it, we chain keypeek_handle_command()
// from our own raw_hid_receive_kb in key_colors.c instead.
#define KEYPEEK_DISABLE_RAW_HID_HANDLER
// --- end keypeek ---
