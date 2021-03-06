#include "kbd75.h"

#include "raw_hid.h"
#include "rgblight.h"

#define CODE_OK 1
#define CODE_UNHANDLED 2
#define CODE_ERROR 3
#define CODE_INVALID 4

#define ID_PING 1

#define ID_RGBLIGHT_ENABLE_GET 10
#define ID_RGBLIGHT_ENABLE_SET 11
#define ID_RGBLIGHT_HSV_GET 12
#define ID_RGBLIGHT_HSV_SET 13
#define ID_RGBLIGHT_HSV_SET_NOEEPROM 14
#define ID_RGBLIGHT_SET_MULTI_RGB 15
#define ID_RGBLIGHT_SET_MULTI_HSV 16
#define ID_RGBLIGHT_SET_OFFSET_RGB 17
#define ID_RGBLIGHT_SET_OFFSET_HSV 18

#define ID_NKRO_GET 30
#define ID_NKRO_SET 31

#define ID_RESET 60
#define ID_RESET_EEPROM 61

static uint8_t raw_hid_receive_run(uint8_t id, uint8_t *data, uint8_t *response_data) {
    switch (id)
    {
        case ID_RGBLIGHT_ENABLE_GET:
        {
            response_data[0] = rgblight_is_enabled();
            break;
        }
        case ID_RGBLIGHT_ENABLE_SET:
        {
            if (data[1]) {
                rgblight_enable();
            } else {
                rgblight_disable();
            }
            break;
        }
        case ID_RGBLIGHT_SET_MULTI_RGB: // [Count]IRGBIRGBIRGB...
        {
            uint8_t imax = data[0] * 4;
            for (uint8_t i = 0; i < imax; i += 4) {
                setrgb(data[i + 2], data[i + 3], data[i + 4], &led[data[i + 1]]);
            }
            rgblight_set();
            break;
        }
        case ID_RGBLIGHT_SET_MULTI_HSV: // [Count]IHSVIHSVIHSV...
        {
            uint8_t imax = data[0] * 4;
            for (uint8_t i = 0; i < imax; i += 4) {
                sethsv(data[i + 2], data[i + 3], data[i + 4], &led[data[i + 1]]);
            }
            rgblight_set();
            break;
        }
        case ID_RGBLIGHT_SET_OFFSET_RGB: // [Count][Offset]RGBRGBRGB...
        {
            uint8_t imax = data[0] * 3;
            uint8_t ledidx = data[1];
            for (uint8_t i = 0; i < imax; i += 3) {
                setrgb(data[i + 2], data[i + 3], data[i + 4], &led[ledidx]);
                ledidx++;
            }
            rgblight_set();
            break;
        }
        case ID_RGBLIGHT_SET_OFFSET_HSV: // [Count][Offset]HSVHSVHSV...
        {
            uint8_t imax = data[0] * 3;
            uint8_t ledidx = data[1];
            for (uint8_t i = 0; i < imax; i += 3) {
                sethsv(data[i + 2], data[i + 3], data[i + 4], &led[ledidx]);
                ledidx++;
            }
            rgblight_set();
            break;
        }
        case ID_RGBLIGHT_HSV_GET:
        {
            response_data[0] = rgblight_get_hue();
            response_data[1] = rgblight_get_sat();
            response_data[2] = rgblight_get_val();
            break;
        }
        case ID_RGBLIGHT_HSV_SET: // HSV
        {
            rgblight_sethsv(data[0], data[1], data[2]);
            break;
        }
        case ID_RGBLIGHT_HSV_SET_NOEEPROM: // HSV
        {
            rgblight_sethsv_noeeprom(data[0], data[1], data[2]);
            break;
        }
        case ID_NKRO_GET:
        {
            response_data[0] = keymap_config.nkro;
            break;
        }
        case ID_NKRO_SET:
        {
            // Clear prevents stuck keys!
            clear_keyboard();
            keymap_config.nkro = data[0] ? true : false;
            eeconfig_update_keymap(keymap_config.raw);
            clear_keyboard();
            break;
        }
        case ID_RESET:
        {
            reset_keyboard();
            break;
        }
        case ID_RESET_EEPROM:
        {
            eeconfig_init();
            break;
        }
        case ID_PING:
        {
            break;
        }
        default:
        {
            return CODE_UNHANDLED;
        }
    }

    return CODE_OK;
}

void raw_hid_receive(uint8_t *data, uint8_t length) {
    uint8_t id = data[0] & 0x7F;
    uint8_t silent = data[0] & 0x80; // Do not send reply
    data[1] = raw_hid_receive_run(id, data + 1, data + 2);
    if (silent) {
        return;
    }
    raw_hid_send(data, length);
}
