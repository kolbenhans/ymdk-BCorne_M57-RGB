// SPDX-License-Identifier: GPL-2.0-or-later
//
// Split-half sync for the SignalRGB community module (modules/signalrgb).
//
// SignalRGB streams arbitrary per-LED colors over raw HID to the master
// half only. Stock SPLIT_RGB_MATRIX only syncs mode/hsv/speed for the
// built-in algorithmic effects — there's no state to resync an externally
// streamed pixel buffer from, so direct-color modes need their own RPC
// channel. This mirrors the same approach the old dynamic_lights.c used
// for VialRGB direct mode / viz_frame.

#include QMK_KEYBOARD_H
#include <string.h>
#include "signalrgb_split.h"

RGB signalrgb_colors[RGB_MATRIX_LED_COUNT];

#define SYNC_HALF_SIZE (RGB_MATRIX_LED_COUNT / 2)

static void signalrgb_sync_handler(uint8_t in_buflen, const void *in_data, uint8_t out_buflen, void *out_data) {
    (void)out_buflen;
    (void)out_data;
    if (in_buflen != SYNC_HALF_SIZE * sizeof(RGB) || in_data == NULL) return;
    uint8_t local_offset = is_keyboard_left() ? 0 : SYNC_HALF_SIZE;
    memcpy(&signalrgb_colors[local_offset], in_data, in_buflen);
}

void keyboard_post_init_user(void) {
    transaction_register_rpc(USER_SYNC_SIGNALRGB, signalrgb_sync_handler);
}

void housekeeping_task_user(void) {
    if (!is_keyboard_master()) return;
    if (rgb_matrix_get_mode() != RGB_MATRIX_COMMUNITY_MODULE_SIGNALRGB) return;

    static uint32_t last_sync = 0;
    if (timer_elapsed32(last_sync) < 20) return;
    last_sync = timer_read32();

    // LED indices 0..SYNC_HALF_SIZE-1 are always the physical left half
    // (fixed by g_led_config) — is_keyboard_left() picks the right offset
    // regardless of which physical side is master.
    uint8_t remote_offset = is_keyboard_left() ? SYNC_HALF_SIZE : 0;
    transaction_rpc_send(USER_SYNC_SIGNALRGB, SYNC_HALF_SIZE * sizeof(RGB), &signalrgb_colors[remote_offset]);
}

bool rgb_matrix_indicators_user(void) {
    if (rgb_matrix_get_mode() != RGB_MATRIX_COMMUNITY_MODULE_SIGNALRGB) return true;

    uint8_t local_min = is_keyboard_left() ? 0 : SYNC_HALF_SIZE;
    uint8_t local_max = local_min + SYNC_HALF_SIZE;
    for (uint8_t i = local_min; i < local_max; i++) {
        rgb_matrix_set_color(i, signalrgb_colors[i].r, signalrgb_colors[i].g, signalrgb_colors[i].b);
    }
    return true;
}
