ｊ/*
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

// ▼▼▼ ここから追加 ▼▼▼
#ifdef POINTING_DEVICE_ENABLE
#  include "drivers/sensors/pmw3360.h"
#endif
// ▲▲▲ ここまで追加 ▲▲▲

#include "quantum.h"

// ▼▼▼ ここから追加 ▼▼▼
static uint16_t cpi_values[] = { 400, 800, 1200, 1600, 2400 }; // 好みの速度リスト
static uint8_t cpi_idx = 1; // 初期値（0なら400, 1なら800...）
// ▲▲▲ ここまで追加 ▲▲▲

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
  // ▼▼▼ ここから追加 ▼▼▼
  switch (keycode) {
    case KC_F24: // Remapで F24 を割り当てたキーが「DPI切り替え」になります
      if (record->event.pressed) {
        cpi_idx = (cpi_idx + 1) % (sizeof(cpi_values) / sizeof(cpi_values[0]));
        #ifdef POINTING_DEVICE_ENABLE
          pmw_set_cpi(cpi_values[cpi_idx]);
        #endif
      }
      return false; // PCにはF24キーを入力しない
  }
  // ▲▲▲ ここまで追加 ▲▲▲

  return true;
}
