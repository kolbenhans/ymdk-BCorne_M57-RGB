#pragma once

#include "color.h"

// Full-board direct-color buffer fed by modules/signalrgb/signalrgb.c
// (signalrgb_paint). Indices 0..RGB_MATRIX_LED_COUNT-1, physical layout
// fixed by g_led_config regardless of which half is USB/master.
extern RGB signalrgb_colors[RGB_MATRIX_LED_COUNT];
