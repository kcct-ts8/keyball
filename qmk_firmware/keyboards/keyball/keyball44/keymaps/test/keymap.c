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
//const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
//  [0] = LAYOUT_universal(
//    KC_ESC   , KC_Q     , KC_W     , KC_E     , KC_R     , KC_T     ,                                        KC_Y     , KC_U     , KC_I     , KC_O     , KC_P     , KC_MINS  ,
//    KC_TAB   , KC_A     , KC_S     , KC_D     , KC_F     , KC_G     ,                                        KC_H     , KC_J     , KC_K     , KC_L     , KC_SCLN  , KC_QUOT  ,
//    KC_LSFT  , KC_Z     , KC_X     , KC_C     , KC_V     , KC_B     ,                                        KC_N     , KC_M     , KC_COMM  , KC_DOT   , KC_SLSH  , KC_RSFT  ,
//                    KC_LCTL  , KC_LGUI  , KC_LALT  ,       KC_SPC   , KC_DEL   ,                  KC_BSPC  , KC_ENT   ,      KC_RALT  , KC_RGUI  , KC_RCTL
//  ),
//};
// clang-format on

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  // =========================================================
  // Layer 0: 基本配列 (Base)
  // =========================================================
  [0] = LAYOUT_universal(
    KC_TAB , KC_Q   , KC_W   , KC_E   , KC_R   , KC_T   ,                                      KC_Y   , KC_U   , KC_I   , KC_O   , KC_P   , KC_RBRC,
    KC_LCTL, KC_A   , KC_S   , KC_D   , KC_F   , KC_G   ,                                      KC_H   , KC_J   , KC_K   , KC_L   , LT(1, KC_COMM), KC_BSLS,
    KC_LSFT, KC_Z   , KC_X   , KC_C   , KC_V   , KC_B   ,                                      KC_N   , KC_M   , KC_COMM, KC_DOT , KC_SLSH, TG(1)  ,
             LT(3, KC_BSPC), LT(2, KC_ESC), LT(1, KC_LALT), KC_LGUI, LT(4, KC_SPC),    _______, _______, LT(5, KC_MINS), LT(2, KC_ENT), LT(3, KC_LBRC)
  ),

  // =========================================================
  // Layer 1: マウス操作・メディア・独自ショートカット
  // =========================================================
  [1] = LAYOUT_universal(
    _______, SGUI(KC_1), SGUI(KC_2), LCTL(SGUI(KC_3)), LCTL(SGUI(KC_4)), LCTL(KC_T),           _______, KC_F17 , KC_F19 , KC_F18 , KC_F13 , KC_F12 ,
    PRC_SW , PRC_TOG   , _______   , LALT(KC_C)      , LCTL(SGUI(KC_D)), KC_F15    ,           KC_BSPC, KC_MS_BTN1, SCRL_MO, KC_MS_BTN2, KC_F16 , KC_F11 ,
    _______, _______   , _______   , _______         , LCTL(LALT(KC_SPC)), KC_F14  ,           KC_VOLD, KC_VOLU, KC_MS_BTN3, KC_MUTE, _______, _______,
             _______, _______, _______, _______, _______,                              _______, _______, _______, _______, _______
  ),

  // =========================================================
  // Layer 2: 記号・テンキー補助
  // =========================================================
  [2] = LAYOUT_universal(
    _______, _______, _______, _______, _______, _______,                                      _______, S(KC_INT1), S(KC_7), S(KC_8), S(KC_9), S(KC_0),
    _______, _______, _______, _______, _______, _______,                                      KC_BSPC, KC_PLUS, KC_MINS, S(KC_BSLS),_______, _______,
    _______, _______, _______, _______, _______, _______,                                      KC_PPLS, KC_PMNS, _______, KC_PAST, _______, _______,
             _______, _______, _______, _______, _______,                              _______, _______, _______, _______, _______
  ),

  // =========================================================
  // Layer 3: RGB LED コントロール & Keyball設定
  // =========================================================
  [3] = LAYOUT_universal(
    RGB_TOG , RGB_M_P , RGB_M_B , RGB_M_R , RGB_M_SW, _______,                                 AML_TO , AML_I50, AML_D50, _______, _______, KBC_SAVE,
    RGB_MOD , RGB_HUI , RGB_SAI , RGB_VAI , RGB_M_SN, _______,                                 _______, _______, _______, _______, _______, SCRL_DVI,
    RGB_RMOD, RGB_HUD , RGB_SAD , RGB_VAD , RGB_M_K , _______,                                 CPI_D1K, CPI_D100, CPI_I100, CPI_I1K, _______, SCRL_DVD,
             _______, _______, _______, _______, _______,                              _______, _______, _______, _______, _______
  ),

  // =========================================================
  // Layer 4: ファンクションキー (F1〜F10) & 数字
  // =========================================================
  [4] = LAYOUT_universal(
    _______, KC_1   , KC_2   , KC_3   , KC_4   , KC_5   ,                                      KC_6   , KC_7   , KC_8   , KC_9   , KC_0   , _______,
    _______, KC_F1  , KC_F2  , KC_F3  , KC_F4  , KC_F5  ,                                      KC_F6  , KC_F7  , KC_F8  , KC_F9  , KC_F10 , _______,
    _______, _______, _______, _______, _______, _______,                                      _______, _______, _______, _______, _______, _______,
             _______, _______, _______, _______, _______,                              _______, _______, _______, _______, _______
  ),

  // =========================================================
  // Layer 5: テンキー (Numpad)
  // =========================================================
  [5] = LAYOUT_universal(
    _______, _______, _______, _______, _______, _______,                                      KC_PEQL, KC_P1  , KC_P2  , KC_P3  , KC_PPLS, _______,
    _______, _______, _______, _______, _______, _______,                                      KC_PMNS, KC_P4  , KC_P5  , KC_P6  , KC_PMNS, _______,
    _______, _______, _______, _______, _______, _______,                                      KC_PAST, KC_P7  , KC_P8  , KC_P9  , KC_PSLS, _______,
             _______, _______, _______, _______, _______,                              _______, _______, KC_P0  , KC_PDOT, _______
  )
};

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

#endif
// ==========================================
// ▲▲▲ コンボの設定ここまで ▲▲▲
// ==========================================
