#include QMK_KEYBOARD_H

const rgblight_segment_t PROGMEM my_capslock_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {12, 4, HSV_WHITE}
);

const rgblight_segment_t PROGMEM my_layer1_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {8, 4, HSV_BLUE}
);

const rgblight_segment_t* const PROGMEM my_rgb_layers[] = RGBLIGHT_LAYERS_LIST(
    my_capslock_layer,
    my_layer1_layer
);

void keyboard_post_init_kb(void) {
    rgblight_layers = my_rgb_layers;
    rgblight_reload_from_eeprom();
    keyboard_post_init_user();
}

bool led_update_kb(led_t led_state) {
    if(led_update_user(led_state)) {
        rgblight_set_layer_state(0, led_state.caps_lock);
    }
    return true;
}

layer_state_t layer_state_set_kb(layer_state_t state) {
    state = layer_state_set_user(state);
    rgblight_set_layer_state(1, layer_state_cmp(state, 1));
    return state;
}
