RGB_MATRIX_EFFECT(key_colors)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

extern void key_colors_render(uint8_t led_min, uint8_t led_max);

static bool key_colors(effect_params_t *params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);
    key_colors_render(led_min, led_max);
    return rgb_matrix_check_finished_leds(led_max);
}

#endif
