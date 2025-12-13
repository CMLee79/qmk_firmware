#include QMK_KEYBOARD_H

// Layers
enum layers {
    _L0,
    _L1,
    _L2
};

// Keymap (예시)
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_L0] = { { KC_A, KC_B, KC_C, KC_D } },
    [_L1] = { { KC_1, KC_2, KC_3, KC_4 } },
    [_L2] = { { KC_F1, KC_F2, KC_F3, KC_F4 } }
};

// --- ENTER MACRO: 500ms ±50ms toggle ---
static bool enter_macro_active = false;
static uint32_t enter_macro_timer = 0;  // uint32_t로 변경

// --- Layer switch keys ---
static bool p0 = false, p3 = false;
static bool layer_switched = false;  // 동시에 눌림 1회 트리거 방지

// Unified blink (used from layer_state_set_user)
static void blink_layer_once_count(uint8_t count) {
    // NOTE: using short blocking waits; if responsiveness is critical, convert to non-blocking.
    for (uint8_t i = 0; i < count; ++i) {
        rgb_matrix_set_color_all(255, 255, 255);
        wait_ms(120);
        rgb_matrix_set_color_all(0, 0, 0);
        wait_ms(120);
    }
    // Ensure PWM buffers are updated (if available)
#ifdef RGB_MATRIX_ENABLE
    rgb_matrix_update_pwm_buffers();
#endif
}

// Called when layer state changes; single place to trigger blink
layer_state_t layer_state_set_user(layer_state_t state) {
    uint8_t layer = get_highest_layer(state);
    // Blink layer times (layer 0 -> 1 blink, layer 1 -> 2 blinks, etc.)
    blink_layer_once_count(layer + 1);
    return state;
}

// process_record_user: handles both layer-cycle combo and F13 toggle
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // Update physical key pressed state for row/col combo detection
    if (record->event.key.row == 0 && record->event.key.col == 0) {
        p0 = record->event.pressed;
    }
    if (record->event.key.row == 0 && record->event.key.col == 3) {
        p3 = record->event.pressed;
    }

    // Combo: row0col0 + row0col3 => next layer (wrap)
    if (p0 && p3) {
        if (!layer_switched) {
            uint8_t next = (biton32(layer_state) + 1) % 3;
            layer_move(next);        // this will call layer_state_set_user -> blink
            layer_switched = true;
        }
    } else {
        layer_switched = false;
    }

    // F13: toggle enter macro on any layer
    if (keycode == KC_F13 && record->event.pressed) {
        enter_macro_active = !enter_macro_active;
        enter_macro_timer = timer_read(); // 32-bit timer
        return false;  // swallow F13 default
    }

    return true;
}

// matrix_scan_user: drive the enter macro timing
void matrix_scan_user(void) {
    if (enter_macro_active) {
        // Produce pseudo-random jitter using timer_read() to avoid rand()
        uint32_t jitter = (timer_read() % 101); // 0..100
        int32_t offset = (int32_t)jitter - 50;  // -50..+50
        uint32_t interval = 500 + offset;       // 450..550

        if (timer_elapsed(enter_macro_timer) >= interval) {
            enter_macro_timer = timer_read();
            tap_code(KC_ENTER); // KC_ENTER 권장
        }
    }
}#include QMK_KEYBOARD_H

// Layers
enum layers {
    _L0,
    _L1,
    _L2
};

// Keymap (예시)
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_L0] = { { KC_A, KC_B, KC_C, KC_D } },
    [_L1] = { { KC_1, KC_2, KC_3, KC_4 } },
    [_L2] = { { KC_F1, KC_F2, KC_F3, KC_F4 } }
};

// --- ENTER MACRO: 500ms ±50ms toggle ---
static bool enter_macro_active = false;
static uint32_t enter_macro_timer = 0;  // uint32_t로 변경

// --- Layer switch keys ---
static bool p0 = false, p3 = false;
static bool layer_switched = false;  // 동시에 눌림 1회 트리거 방지

// Unified blink (used from layer_state_set_user)
static void blink_layer_once_count(uint8_t count) {
    // NOTE: using short blocking waits; if responsiveness is critical, convert to non-blocking.
    for (uint8_t i = 0; i < count; ++i) {
        rgb_matrix_set_color_all(255, 255, 255);
        wait_ms(120);
        rgb_matrix_set_color_all(0, 0, 0);
        wait_ms(120);
    }
    // Ensure PWM buffers are updated (if available)
#ifdef RGB_MATRIX_ENABLE
    rgb_matrix_update_pwm_buffers();
#endif
}

// Called when layer state changes; single place to trigger blink
layer_state_t layer_state_set_user(layer_state_t state) {
    uint8_t layer = get_highest_layer(state);
    // Blink layer times (layer 0 -> 1 blink, layer 1 -> 2 blinks, etc.)
    blink_layer_once_count(layer + 1);
    return state;
}

// process_record_user: handles both layer-cycle combo and F13 toggle
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // Update physical key pressed state for row/col combo detection
    if (record->event.key.row == 0 && record->event.key.col == 0) {
        p0 = record->event.pressed;
    }
    if (record->event.key.row == 0 && record->event.key.col == 3) {
        p3 = record->event.pressed;
    }

    // Combo: row0col0 + row0col3 => next layer (wrap)
    if (p0 && p3) {
        if (!layer_switched) {
            uint8_t next = (biton32(layer_state) + 1) % 3;
            layer_move(next);        // this will call layer_state_set_user -> blink
            layer_switched = true;
        }
    } else {
        layer_switched = false;
    }

    // F13: toggle enter macro on any layer
    if (keycode == KC_F13 && record->event.pressed) {
        enter_macro_active = !enter_macro_active;
        enter_macro_timer = timer_read(); // 32-bit timer
        return false;  // swallow F13 default
    }

    return true;
}

// matrix_scan_user: drive the enter macro timing
void matrix_scan_user(void) {
    if (enter_macro_active) {
        // Produce pseudo-random jitter using timer_read() to avoid rand()
        uint32_t jitter = (timer_read() % 101); // 0..100
        int32_t offset = (int32_t)jitter - 50;  // -50..+50
        uint32_t interval = 500 + offset;       // 450..550

        if (timer_elapsed(enter_macro_timer) >= interval) {
            enter_macro_timer = timer_read();
            tap_code(KC_ENTER); // KC_ENTER 권장
        }
    }
}