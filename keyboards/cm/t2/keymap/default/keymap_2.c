#include QMK_KEYBOARD_H

// -------------------------------------------------------------
// Layers
// -------------------------------------------------------------
enum layers {
    _L0,
    _L1,
    _L2
};

// Keymap
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_L0] = { { KC_A, KC_B, KC_C, KC_D } },
    [_L1] = { { KC_1, KC_2, KC_3, KC_4 } },
    [_L2] = { { KC_F1, KC_F2, KC_F3, KC_F4 } }
};

// -------------------------------------------------------------
// F13 ENTER 매크로
// -------------------------------------------------------------
static bool enter_macro_active = false;
static uint16_t enter_macro_timer = 0;

// 매크로 ON 상태 LED 강제 표시
static bool macro_led_override = false;

// -------------------------------------------------------------
// 논블로킹 레이어 LED 깜빡임
// -------------------------------------------------------------
static bool layer_blinking = false;
static uint8_t layer_blink_count = 0;
static uint8_t layer_blink_target = 0;
static uint16_t layer_blink_timer = 0;
static bool layer_blink_state = false;   // ON/OFF 상태

void start_layer_blink(uint8_t times) {
    layer_blinking = true;
    layer_blink_count = 0;
    layer_blink_target = times * 2;  // ON/OFF 한 쌍 → 2회
    layer_blink_timer = timer_read();
}

// -------------------------------------------------------------
// 레이어 전환 → 논블로킹 깜빡임 트리거
// -------------------------------------------------------------
layer_state_t layer_state_set_user(layer_state_t state) {
    uint8_t layer = get_highest_layer(state);
    start_layer_blink(layer + 1);
    return state;
}

// -------------------------------------------------------------
// F13 매크로 / 레이어 전환키 처리
// -------------------------------------------------------------
static bool p0 = false, p3 = false;
static bool layer_switched = false;
// -------------------------------------------------------------
// Layer switching: require holding p0+p3 for > 2000ms
// -------------------------------------------------------------
static uint16_t layer_hold_timer = 0;
static bool layer_hold_started = false;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {

    // --- 레이어 전환용 키 (row/col 감지) ---
    if (record->event.key.row == 0 && record->event.key.col == 0) {
        p0 = record->event.pressed;
    }
    if (record->event.key.row == 0 && record->event.key.col == 3) {
        p3 = record->event.pressed;
    }

    // 두 키가 동시에 눌렸을 때
    if (p0 && p3) {
        // 처음 눌린 순간 타이머 시작
        if (!layer_hold_started) {
            layer_hold_started = true;
            layer_hold_timer = timer_read();
        }

        // 3초(3000ms) 이상 유지된 경우 1회 레이어 전환
        if (!layer_switched && timer_elapsed(layer_hold_timer) > 3000) {
            uint8_t next = (biton32(layer_state) + 1) % 3;
            layer_move(next);
            layer_switched = true;   // 재반복 방지
        }
    } 
    else {
        // 키 중 하나라도 떼면 초기화
        layer_switched = false;
        layer_hold_started = false;
    }

    // --- F13 매크로 토글 ---
    if (keycode == KC_F13 && record->event.pressed) {
        enter_macro_active = !enter_macro_active;
        enter_macro_timer = timer_read();
        macro_led_override = enter_macro_active;    // LED 강제 ON/OFF 모드 진입
        return false;
    }

    return true;
}

// -------------------------------------------------------------
// MATRIX SCAN (논블로킹)
// -------------------------------------------------------------
void matrix_scan_user(void) {

    // ================================
    // 1) ENTER 매크로 실행
    // ================================
    if (enter_macro_active) {
        if (timer_elapsed(enter_macro_timer) >
            (500 + (rand() % 101 - 50))) {
            
            enter_macro_timer = timer_read();
            tap_code(KC_ENT);
        }
    }

    // ================================
    // 2) 레이어 LED 깜빡임 (논블로킹)
    // ================================
    if (layer_blinking) {
        if (timer_elapsed(layer_blink_timer) > 120) {
            layer_blink_timer = timer_read();
            layer_blink_state = !layer_blink_state;

            if (layer_blink_state) {
                rgb_matrix_set_color_all(255, 255, 255);
            } else {
                rgb_matrix_set_color_all(0, 0, 0);
            }

            layer_blink_count++;
            if (layer_blink_count >= layer_blink_target) {
                layer_blinking = false;
                rgb_matrix_enable();
            }
        }
    }

    // ================================
    // 3) F13 매크로 LED 강제 표시
    // ================================
    if (macro_led_override) {
        // 매크로 ON → F13 위치를 Red로 표시
        // 실제 키 위치를 알면 변경 (예: row0 col2 라고 가정)
        rgb_matrix_set_color(0, 2, 255, 0, 0);
    }
}