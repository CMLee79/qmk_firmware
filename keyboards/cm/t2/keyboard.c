#include "quantum.h"

// LED index mapping for RGB Matrix
led_config_t g_led_config = {
    {
        {0, 1, 2, 3}
    },
    {
        {0, 0}, {32, 0}, {64, 0}, {96, 0}
    },
    {
        LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT, LED_FLAG_KEYLIGHT
    }
};