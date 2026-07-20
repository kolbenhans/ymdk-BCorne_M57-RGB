#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
     * Layer 0: Base QWERTY
     * Corne-inspired default layer with number row.
     */
    [0] = LAYOUT(
        KC_ESC,   KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                          KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
        KC_TAB,   KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_UP,      KC_DOWN,  KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC,
        KC_CAPS,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_LEFT,    KC_RGHT,  KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
        KC_LSFT,  KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_MUTE,    RGB_MOD,  KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_ENT,
                                      KC_LALT, MO(1),  KC_SPC,                       KC_ENT,  MO(2),   KC_RCTL
    ),

    /*
     * Layer 1: Symbols / navigation / function keys
     */
    [1] = LAYOUT(
        KC_GRV,   KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                        KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_DEL,
        _______,  KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_PGUP,    KC_HOME,  KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_RBRC,
        _______,  KC_LCTL, KC_LGUI, KC_LALT, KC_PIPE, KC_BSLS, KC_PGDN,    KC_END,   KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, KC_MINS, KC_EQL,
        _______,  _______, _______, _______, _______, _______, KC_MUTE,    RGB_TOG,  KC_UNDS, KC_PLUS, KC_LCBR, KC_RCBR, KC_COLN, KC_DQUO,
                                      _______, _______, _______,                     _______, MO(3),   _______
    ),

    /*
     * Layer 2: Numpad / F11-F12 / extra navigation
     */
    [2] = LAYOUT(
        KC_ESC,   KC_F11,  KC_F12,  _______, _______, _______,                      KC_NUM, _______, _______, _______,   _______, _______,
        _______,  _______, _______, _______, _______, _______, _______,    _______,  KC_PPLS, KC_P7,   KC_P8,   KC_P9,   KC_PAST, _______,
        _______,  _______, _______, _______, _______, _______, _______,    _______,  KC_PMNS, KC_P4,   KC_P5,   KC_P6,   KC_PSLS, _______,
        _______,  _______, _______, _______, _______, _______, KC_MUTE,    _______,  _______, KC_P1,   KC_P2,   KC_P3,   KC_ENT,  _______,
                                      _______, MO(3),   _______,                    _______, KC_P0, _______
    ),

    /*
     * Layer 3: System / RGB / bootloader
     */
    [3] = LAYOUT(
        QK_BOOT,  _______, _______, _______, _______, _______,                      _______, _______, _______, _______, _______, QK_BOOT,
        _______,  RGB_TOG, RGB_MOD, RGB_HUI, RGB_SAI, RGB_VAI, _______,   _______,  _______, _______, _______, _______, _______, _______,
        _______,  RGB_RMOD,RGB_HUD, RGB_SAD, RGB_VAD, RGB_SPI, _______,   _______,  _______, _______, _______, _______, _______, _______,
        _______,  EE_CLR,  _______, _______, _______, _______, KC_MUTE,   RGB_MOD,  _______, _______, _______, _______, _______, QK_BOOT,
                                      _______, _______, _______,                    _______, _______, _______
    )
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][2] = {
    [0] = { ENCODER_CCW_CW(KC_LEFT,  KC_RIGHT), ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [1] = { ENCODER_CCW_CW(KC_PGDN,  KC_PGUP),  ENCODER_CCW_CW(RGB_HUD, RGB_HUI) },
    [2] = { ENCODER_CCW_CW(KC_DOWN,  KC_UP),    ENCODER_CCW_CW(RGB_VAD, RGB_VAI) },
    [3] = { ENCODER_CCW_CW(RGB_RMOD, RGB_MOD),  ENCODER_CCW_CW(RGB_SPD, RGB_SPI) },
};
#endif