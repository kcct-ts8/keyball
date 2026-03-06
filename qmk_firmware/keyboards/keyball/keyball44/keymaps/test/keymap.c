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

// 普段の速度（16 = 1600 DPI）
#define NORMAL_CPI_VAL 16
// 精密モードの速度（6 = 600 DPI）
#define PRECISION_CPI_VAL 6

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

void keyboard_post_init_user(void) {
#ifdef RGBLIGHT_ENABLE
    // LED設定をEEPROM（メモリ）に保存しない設定を無効化（＝保存するようにする）
    // ここをコメントアウトしておくと、Remapで決めた色が再起動後も保持されます
    // rgblight_enable_noeeprom();
#endif

#ifdef POINTING_DEVICE_ENABLE
    // 起動時にデフォルト速度を適用
    keyball_set_cpi(NORMAL_CPI_VAL);
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
          // キーを押した時
          keyball_set_cpi(PRECISION_CPI_VAL);
        } else {
          // キーを離した時
          if (!is_precision_mode) {
             keyball_set_cpi(NORMAL_CPI_VAL);
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
            keyball_set_cpi(NORMAL_CPI_VAL);
            is_precision_mode = false;
          } else {
            // 今が普段モードなら -> 精密にする
            keyball_set_cpi(PRECISION_CPI_VAL);
            is_precision_mode = true;
          }
        }
      #endif
      return false;
      
    // 他のキーコード処理があればここに続く...
  }
  
  return true;
}


// ==========================================
// ▼▼▼ コンボ（同時押し）の設定 ▼▼▼
// ==========================================
#ifdef COMBO_ENABLE

// 1. コンボに名前をつける
enum combos {
  CMB_ESC,
  CMB_ENT,
  CMB_BSPC,
  CMB_DEL,
};

// 2. 同時押しするキーの組み合わせを定義する（必ず最後に COMBO_END をつける）
const uint16_t PROGMEM combo_esc[]  = {KC_J, KC_K, COMBO_END};
const uint16_t PROGMEM combo_ent[]  = {KC_K, KC_L, COMBO_END};
const uint16_t PROGMEM combo_bspc[] = {KC_U, KC_I, COMBO_END};
const uint16_t PROGMEM combo_del[]  = {KC_O, KC_P, COMBO_END};

// 3. 組み合わせと、発動するキーを紐付ける
combo_t key_combos[] = {
  [CMB_ESC]  = COMBO(combo_esc, KC_ESC),   // J + K で Esc
  [CMB_ENT]  = COMBO(combo_ent, KC_ENT),   // K + L で Enter
  [CMB_BSPC] = COMBO(combo_bspc, KC_BSPC), // U + I で Backspace
  [CMB_DEL]  = COMBO(combo_del, KC_DEL),   // O + P で Delete
};

// 同時押しの判定時間（ミリ秒）。誤爆する場合はこの数字を減らします。
uint16_t COMBO_TERM = 50; 

#endif
// ==========================================
// ▲▲▲ コンボの設定ここまで ▲▲▲
// ==========================================
