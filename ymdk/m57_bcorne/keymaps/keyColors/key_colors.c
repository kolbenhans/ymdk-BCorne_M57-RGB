#include QMK_KEYBOARD_H
#include "transactions.h"
#include "key_colors.h"
#include "keypeek_layer_notify.h" // keypeek
#include <string.h>

#if defined(RGB_MATRIX_ENABLE)

// ─── Hardware constants ───────────────────────────────────────────────────────

#define KEY_LED_COUNT 58

// ─── Startup animation ───────────────────────────────────────────────────────

#define STARTUP_STEP_MS    10
#define STARTUP_TAIL_WIDTH 84

// ─── Per-key/per-layer color table ────────────────────────────────────────────
// Host (WebGUI) owns this data: assigned via Raw HID 0x02/0xA5, persisted via
// 0x02/0xA6, read back via 0x02/0xA7. Firmware never derives color from
// keycode/keymap content — only from LED index + active layer.

typedef struct { uint8_t r, g, b; } rgb_color_t;

static rgb_color_t key_colors[DYNAMIC_KEYMAP_LAYER_COUNT][KEY_LED_COUNT];
static rgb_color_t blink_colors[DYNAMIC_KEYMAP_LAYER_COUNT][KEY_LED_COUNT];
static uint8_t      key_lock_flags[DYNAMIC_KEYMAP_LAYER_COUNT][KEY_LED_COUNT];
static bool         colors_dirty = false;

// Set true whenever any LED on any layer carries LOCK_FLAG_BLINK — gates the
// 800ms phase-flip check in render_lighting_range() so boards with no blink
// keys configured never pay the extra cache_rebuild()/send_light_sync() tax.
static bool any_blink = false;

static void recompute_any_blink(void) {
    any_blink = false;
    for (uint8_t layer = 0; layer < DYNAMIC_KEYMAP_LAYER_COUNT && !any_blink; layer++) {
        for (uint8_t led = 0; led < KEY_LED_COUNT; led++) {
            if (key_lock_flags[layer][led] & LOCK_FLAG_BLINK) {
                any_blink = true;
                break;
            }
        }
    }
}

// 58 LEDs split evenly in half (29 each) — matches this board's existing
// dynamic_lights.c split-sync approach (SYNC_HALF_SIZE), so RPC_M2S_BUFFER_SIZE
// is already sized for it (29 * 3 = 87 bytes, board config.h sets it to 96).
#define SYNC_HALF_SIZE (KEY_LED_COUNT / 2)
#define SYNC_HALF_BYTES (SYNC_HALF_SIZE * sizeof(rgb_color_t))

static rgb_color_t synced_colors[KEY_LED_COUNT];
static bool         synced_colors_valid = false;

static struct {
    uint32_t anim_timer;
    bool     done;
} startup;

static struct {
    rgb_color_t   colors[KEY_LED_COUNT];
    layer_state_t layer_state;
    uint8_t       led_state_raw; // host_keyboard_led_state().raw — for lock-flag gating
    uint8_t       blink_phase;   // (timer_read32()/800)%2 as of last rebuild
    bool          valid;
} cache;

// Stateless — recomputed on demand, no stored animation timer. 800ms/phase =
// 1.6s full cycle, same cadence as the legacy dynamicLights keymap's
// slow_blink_pick().
static rgb_color_t slow_blink_pick(rgb_color_t a, rgb_color_t b) {
    return (timer_read32() / 800) % 2 ? a : b;
}

// ─── Host API ─────────────────────────────────────────────────────────────────

static void load_colors(void) {
    if (eeconfig_is_kb_datablock_valid()) {
        eeconfig_read_kb_datablock(key_colors, 0, sizeof(key_colors));
        eeconfig_read_kb_datablock(blink_colors, sizeof(key_colors), sizeof(blink_colors));
        eeconfig_read_kb_datablock(key_lock_flags, sizeof(key_colors) + sizeof(blink_colors), sizeof(key_lock_flags));
    } else {
        memset(key_colors, 0, sizeof(key_colors));
        memset(blink_colors, 0, sizeof(blink_colors));
        memset(key_lock_flags, 0, sizeof(key_lock_flags));
    }
    recompute_any_blink();
}

void key_colors_set_colors(uint8_t layer, uint8_t led_offset, uint8_t count, const uint8_t *rgb_bytes) {
    if (layer >= DYNAMIC_KEYMAP_LAYER_COUNT) return;
    for (uint8_t i = 0; i < count && (uint16_t)led_offset + i < KEY_LED_COUNT; i++) {
        rgb_color_t *c = &key_colors[layer][led_offset + i];
        c->r = rgb_bytes[i * 3 + 0];
        c->g = rgb_bytes[i * 3 + 1];
        c->b = rgb_bytes[i * 3 + 2];
    }
    colors_dirty = true;
}

void key_colors_get_colors(uint8_t layer, uint8_t led_offset, uint8_t count, uint8_t *out_rgb_bytes) {
    memset(out_rgb_bytes, 0, (size_t)count * 3);
    if (layer >= DYNAMIC_KEYMAP_LAYER_COUNT) return;
    for (uint8_t i = 0; i < count && (uint16_t)led_offset + i < KEY_LED_COUNT; i++) {
        rgb_color_t c              = key_colors[layer][led_offset + i];
        out_rgb_bytes[i * 3 + 0]   = c.r;
        out_rgb_bytes[i * 3 + 1]   = c.g;
        out_rgb_bytes[i * 3 + 2]   = c.b;
    }
}

void key_colors_set_blink_colors(uint8_t layer, uint8_t led_offset, uint8_t count, const uint8_t *rgb_bytes) {
    if (layer >= DYNAMIC_KEYMAP_LAYER_COUNT) return;
    for (uint8_t i = 0; i < count && (uint16_t)led_offset + i < KEY_LED_COUNT; i++) {
        rgb_color_t *c = &blink_colors[layer][led_offset + i];
        c->r = rgb_bytes[i * 3 + 0];
        c->g = rgb_bytes[i * 3 + 1];
        c->b = rgb_bytes[i * 3 + 2];
    }
    colors_dirty = true;
}

void key_colors_get_blink_colors(uint8_t layer, uint8_t led_offset, uint8_t count, uint8_t *out_rgb_bytes) {
    memset(out_rgb_bytes, 0, (size_t)count * 3);
    if (layer >= DYNAMIC_KEYMAP_LAYER_COUNT) return;
    for (uint8_t i = 0; i < count && (uint16_t)led_offset + i < KEY_LED_COUNT; i++) {
        rgb_color_t c              = blink_colors[layer][led_offset + i];
        out_rgb_bytes[i * 3 + 0]   = c.r;
        out_rgb_bytes[i * 3 + 1]   = c.g;
        out_rgb_bytes[i * 3 + 2]   = c.b;
    }
}

void key_colors_commit_colors(void) {
    eeconfig_update_kb_datablock(key_colors, 0, sizeof(key_colors));
    eeconfig_update_kb_datablock(blink_colors, sizeof(key_colors), sizeof(blink_colors));
    eeconfig_update_kb_datablock(key_lock_flags, sizeof(key_colors) + sizeof(blink_colors), sizeof(key_lock_flags));
}

void key_colors_set_lock_flags(uint8_t layer, uint8_t led, uint8_t flags) {
    if (layer >= DYNAMIC_KEYMAP_LAYER_COUNT || led >= KEY_LED_COUNT) return;
    key_lock_flags[layer][led] = flags;
    colors_dirty = true;
    recompute_any_blink();
}

void key_colors_get_lock_flags(uint8_t layer, uint8_t led_offset, uint8_t count, uint8_t *out_flags) {
    memset(out_flags, 0, count);
    if (layer >= DYNAMIC_KEYMAP_LAYER_COUNT) return;
    for (uint8_t i = 0; i < count && (uint16_t)led_offset + i < KEY_LED_COUNT; i++) {
        out_flags[i] = key_lock_flags[layer][led_offset + i];
    }
}

// ─── Split sync ──────────────────────────────────────────────────────────────
// LED indices 0..SYNC_HALF_SIZE-1 are always the physical left half and
// SYNC_HALF_SIZE..KEY_LED_COUNT-1 the right half (fixed by g_led_config,
// independent of which side is plugged in as master).

static void light_sync_a_handler(uint8_t in_buflen, const void *in_data,
                                  uint8_t out_buflen, void *out_data) {
    if (in_buflen != SYNC_HALF_BYTES || !in_data) return;
    memcpy(&synced_colors[0], in_data, SYNC_HALF_BYTES);
}

static void light_sync_b_handler(uint8_t in_buflen, const void *in_data,
                                  uint8_t out_buflen, void *out_data) {
    if (in_buflen != SYNC_HALF_BYTES || !in_data) return;
    memcpy(&synced_colors[SYNC_HALF_SIZE], in_data, SYNC_HALF_BYTES);
    synced_colors_valid = true;
}

// transaction_rpc_send() silently drops the packet if the split link isn't
// up yet (is_transport_connected() check inside transactions.c) — right
// after boot this can race the startup-comet timing, so the very first sync
// attempt sometimes fails. Track success and keep retrying every tick until
// it goes through once; a real layer/color change always re-triggers anyway.
static bool link_synced = false;

static void send_light_sync(void) {
    bool ok = transaction_rpc_send(USER_SYNC_LIGHTS_A, SYNC_HALF_BYTES, &cache.colors[0]);
    ok      = transaction_rpc_send(USER_SYNC_LIGHTS_B, SYNC_HALF_BYTES, &cache.colors[SYNC_HALF_SIZE]) && ok;
    if (ok) link_synced = true;
}

// ─── Cache ───────────────────────────────────────────────────────────────────
// Repaints from key_colors[] only when the active layer changes, new color/
// lock-flag data arrives over Raw HID (colors_dirty), or the live lock state
// changes (needed for lock-gated LEDs to react instantly) — not on every
// rgb_matrix tick otherwise. Lock-gating is resolved into cache.colors[] here
// (not per-tick in cache_flush_range) so the split-sync path (send_light_sync)
// ships already-resolved colors to the slave half — the slave never needs its
// own host_keyboard_led_state().

static void cache_rebuild(void) {
    uint8_t layer     = get_highest_layer(layer_state);
    led_t   led_state = host_keyboard_led_state();

    for (uint8_t led = 0; led < KEY_LED_COUNT; led++) {
        uint8_t flags   = key_lock_flags[layer][led];
        bool    visible = !(((flags & LOCK_FLAG_NUM)  && !led_state.num_lock) ||
                             ((flags & LOCK_FLAG_CAPS) && !led_state.caps_lock) ||
                             ((flags & LOCK_FLAG_SCRL) && !led_state.scroll_lock));
        if (!visible) {
            cache.colors[led] = (rgb_color_t){0, 0, 0};
        } else if (flags & LOCK_FLAG_BLINK) {
            cache.colors[led] = slow_blink_pick(key_colors[layer][led], blink_colors[layer][led]);
        } else {
            cache.colors[led] = key_colors[layer][led];
        }
    }

    cache.layer_state   = layer_state;
    cache.led_state_raw = led_state.raw;
    cache.blink_phase   = (timer_read32() / 800) % 2;
    cache.valid          = true;
}

static void cache_flush_range(uint8_t led_min, uint8_t led_max) {
    uint8_t v = rgb_matrix_config.hsv.v;
    for (uint8_t led = led_min; led < led_max && led < KEY_LED_COUNT; led++) {
        rgb_color_t c = cache.colors[led];
        rgb_matrix_set_color(led, (uint16_t)c.r * v / 255, (uint16_t)c.g * v / 255, (uint16_t)c.b * v / 255);
    }
}

static void synced_cache_flush_range(uint8_t led_min, uint8_t led_max) {
    if (!synced_colors_valid) return;
    uint8_t v = rgb_matrix_config.hsv.v;
    for (uint8_t led = led_min; led < led_max && led < KEY_LED_COUNT; led++) {
        rgb_color_t c = synced_colors[led];
        rgb_matrix_set_color(led, (uint16_t)c.r * v / 255, (uint16_t)c.g * v / 255, (uint16_t)c.b * v / 255);
    }
}

// ─── Render dispatch ─────────────────────────────────────────────────────────

static void render_lighting_range(uint8_t led_min, uint8_t led_max) {
    if (!is_keyboard_master()) {
        synced_cache_flush_range(led_min, led_max);
        return;
    }

    bool content_stale = !cache.valid || cache.layer_state != layer_state || colors_dirty ||
                          cache.led_state_raw != host_keyboard_led_state().raw ||
                          (any_blink && ((timer_read32() / 800) % 2 != cache.blink_phase));

    if (content_stale) {
        cache_rebuild();
        colors_dirty = false;
    }
    if (content_stale || !link_synced) {
        send_light_sync();
    }

    cache_flush_range(led_min, led_max);
}

// ─── Startup animation ───────────────────────────────────────────────────────

static void startup_tick(uint8_t led_min, uint8_t led_max) {
    if (startup.anim_timer == 0) startup.anim_timer = timer_read32();

    int16_t  head    = (int16_t)(timer_elapsed32(startup.anim_timer) / STARTUP_STEP_MS * 10);
    uint16_t max_pos = 0;

    for (uint8_t led = 0; led < KEY_LED_COUNT; led++) {
        uint8_t  x       = g_led_config.point[led].x;
        uint8_t  y       = g_led_config.point[led].y;
        uint8_t  row     = y / 13;
        uint16_t local_x = (row % 2) ? 220 - x : x;
        uint16_t pos     = (uint16_t)(row * 240) + local_x;
        if (pos > max_pos) max_pos = pos;
    }

    for (uint8_t led = led_min; led < led_max && led < KEY_LED_COUNT; led++) {
        uint8_t  x       = g_led_config.point[led].x;
        uint8_t  y       = g_led_config.point[led].y;
        uint8_t  row     = y / 13;
        uint16_t local_x = (row % 2) ? 220 - x : x;
        uint16_t pos     = (uint16_t)(row * 240) + local_x;
        int16_t  dist    = head - (int16_t)pos;

        if (dist < 0 || dist >= STARTUP_TAIL_WIDTH) {
            rgb_matrix_set_color(led, 0, 0, 0);
            continue;
        }

        uint8_t value = 255 - (uint8_t)((uint16_t)dist * 255 / STARTUP_TAIL_WIDTH);
        uint8_t hue   = (uint8_t)(timer_elapsed32(startup.anim_timer) / 8) + (uint8_t)(pos / 2);
        RGB     rgb   = hsv_to_rgb((HSV){ hue, 255, value });
        rgb_matrix_set_color(led, rgb.r, rgb.g, rgb.b);
    }

    if (head > (int16_t)(max_pos + STARTUP_TAIL_WIDTH)) {
        startup.done = true;
        cache.valid  = false;
    }
}

static void startup_reset(void) {
    startup.anim_timer = 0;
    startup.done       = false;
    cache.valid        = false;
}

static void startup_sync_handler(uint8_t in_buflen, const void *in_data,
                                  uint8_t out_buflen, void *out_data) {
    startup_reset();
}

// ─── Public API ──────────────────────────────────────────────────────────────

void keyboard_post_init_user(void) {
    load_colors();
    transaction_register_rpc(USER_SYNC_LIGHTS_A,      light_sync_a_handler);
    transaction_register_rpc(USER_SYNC_LIGHTS_B,      light_sync_b_handler);
    transaction_register_rpc(USER_KEY_COLORS_STARTUP, startup_sync_handler);
    startup_reset();
}

void key_colors_on_mode_enter(void) {
    startup_reset();
    if (is_keyboard_master()) {
        transaction_rpc_send(USER_KEY_COLORS_STARTUP, 0, NULL);
    }
}

void key_colors_render(uint8_t led_min, uint8_t led_max) {
    if (!startup.done) {
        startup_tick(led_min, led_max);
        return;
    }
    render_lighting_range(led_min, led_max);
}

// ─── Raw HID dispatch — WebGUI key-color/lock-flag commands ─────────────────

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

// WebGUI key-color/lock-flag commands (0xA5-0xA9), sent via WebHID.
void raw_hid_receive_kb(uint8_t *data, uint8_t length) {
    // --- keypeek: claims its own subscribe/keepalive packets, ignores everything else ---
    if (keypeek_handle_command(data, length)) return;
    // --- end keypeek ---

    if (length < 2 || data[0] != 0x02) return;

    switch (data[1]) {
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

#endif // RGB_MATRIX_ENABLE
