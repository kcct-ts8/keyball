/*
Copyright 2022 @Yowkees
Copyright 2022 MURAOKA Taro (aka KoRoN, @kaoriya)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H

#ifdef POINTING_DEVICE_ENABLE
#  include "drivers/sensors/pmw3360.h"
#endif

#include "quantum.h"

// 普段の速度（レベル3 = 1600 DPI）
#define NORMAL_CPI_IDX 16
// 精密モードの速度（レベル0 = 400 DPI）
#define PRECISION_CPI_IDX 6

// ここで既存のキーコード（F23, F24）に別名を付けます
enum my_keycodes {
  PRC_SW = KC_F23, // Remapで「F23」を置くと「押している間だけ精密」になる
  PRC_TOG = KC_F24 // Remapで「F24」を置くと「トグル切り替え」になる
};

// 現在の状態を保存する変数
static bool is_precision_mode = false;

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [0] = LAYOUT_universal(
    KC_ESC   , KC_Q     , KC_W     , KC_E     , KC_R     , KC_T     ,                                        KC_Y     , KC_U     , KC_I     , KC_O     , KC_P     , KC_MINS  ,
    KC_TAB   , KC_A     , KC_S     , KC_D     , KC_F     , KC_G     ,                                        KC_H     , KC_J     , KC_K     , KC_L     , KC_SCLN  , KC_QUOT  ,
    KC_LSFT  , KC_Z     , KC_X     , KC_C     , KC_V     , KC_B     ,                                        KC_N     , KC_M     , KC_COMM  , KC_DOT   , KC_SLSH  , KC_RSFT  ,
                    KC_LCTL  , KC_LGUI  , KC_LALT  ,       KC_SPC   , KC_DEL   ,                  KC_BSPC  , KC_ENT   ,      KC_RALT  , KC_RGUI  , KC_RCTL
  ),
};
// clang-format on

void keyboard_post_init_user() {
#ifdef RGBLIGHT_ENABLE
    // Force RGB lights to show test animation without writing EEPROM.
    rgblight_enable_noeeprom();
    rgblight_mode_noeeprom(RGBLIGHT_MODE_RGB_TEST);
#endif
}

#ifdef OLED_ENABLE

#    include "lib/oledkit/oledkit.h"

void oledkit_render_info_user(void) {
    keyball_oled_render_keyinfo();
    keyball_oled_render_ballinfo();
    keyball_oled_render_layerinfo();
}
#endif
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    
    // ▼▼▼ 押している間だけ精密モード (Switch) ▼▼▼
    case PRC_SW:
      #ifdef POINTING_DEVICE_ENABLE
        if (record->event.pressed) {
          // キーを押した時：精密モード(レベル0)にする
          keyball_set_cpi(PRECISION_CPI_IDX);
        } else {
          // キーを離した時：
          // もしトグルモードで精密化されていなければ、普段の速度(レベル3)に戻す
          if (!is_precision_mode) {
             keyball_set_cpi(NORMAL_CPI_IDX);
          }
        }
      #endif
      return false; // PCにはキー入力を送らない

    // ▼▼▼ 押すたびに切り替え (Toggle) ▼▼▼
    case PRC_TOG:
      #ifdef POINTING_DEVICE_ENABLE
        if (record->event.pressed) {
          if (is_precision_mode) {
            // 今が精密モードなら -> 普段に戻す
            keyball_set_cpi(NORMAL_CPI_IDX);
            is_precision_mode = false;
          } else {
            // 今が普段モードなら -> 精密にする
            keyball_set_cpi(PRECISION_CPI_IDX);
            is_precision_mode = true;
          }
        }
      #endif
      return false;
      
    // 他のキーコード処理があればここに続く...
  }
  return true;
}
