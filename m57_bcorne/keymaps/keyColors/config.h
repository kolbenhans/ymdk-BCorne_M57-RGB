#pragma once

// Distinct from the `vial` keymap's UID so Vial GUI/WebGUI don't confuse
// cached per-keyboard config between keymaps on the same hardware.
#define VIAL_KEYBOARD_UID {0x4B, 0xCB, 0x1D, 0x4D, 0x6E, 0xCA, 0xDA, 0x93}

#define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_COMMUNITY_MODULE_key_colors

// Board default is 10 — this keymap only needs 6, and fewer layers means
// less Vial dynamic-keymap EEPROM pressure too (see budget note below).
#undef DYNAMIC_KEYMAP_LAYER_COUNT
#define DYNAMIC_KEYMAP_LAYER_COUNT 6

// Split sync buffer large enough for 29 LEDs × 3 bytes RGB (87 bytes) —
// same size as the existing `vial` keymap already needs for its own
// split-sync, so this board's WEAR_LEVELING/RPC setup already covers it.
#define RPC_M2S_BUFFER_SIZE 96

// Board default (WEAR_LEVELING_LOGICAL_SIZE 4096 / BACKING_SIZE 8192) is
// already claimed by Vial's own dynamic keymap/combo/tapdance/macro data —
// the extra key_colors datablock doesn't fit on top of that (hits a
// STATIC_ASSERT at compile time). Double both so everything coexists with
// headroom to spare.
#undef WEAR_LEVELING_LOGICAL_SIZE
#undef WEAR_LEVELING_BACKING_SIZE
#define WEAR_LEVELING_LOGICAL_SIZE 8192
#define WEAR_LEVELING_BACKING_SIZE (WEAR_LEVELING_LOGICAL_SIZE * 2)
#undef DYNAMIC_KEYMAP_EEPROM_MAX_ADDR
#define DYNAMIC_KEYMAP_EEPROM_MAX_ADDR (WEAR_LEVELING_LOGICAL_SIZE - 1)

// only if you get "undeclared identifier" (vial-qmk older than
// SPLIT_TRANSACTION_IDS_MODULE_* support):
#define SPLIT_TRANSACTION_IDS_USER \
    KEY_COLORS_COLORS_DELTA, KEY_COLORS_BLINK_DELTA, KEY_COLORS_LOCK_FLAGS_DELTA, \
    KEY_COLORS_COMMIT, KEY_COLORS_STARTUP
