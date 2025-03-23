//
// Created by Wind on 2025/3/20.
//

#include "charge.h"
#include "esp_lvgl_port.h"
#include "lvgl.h"
#include "Gui.h"
#include "HardWare.h"

lv_obj_t *Charge = nullptr;
lv_obj_t *Charge_bar = nullptr;
lv_obj_t *Charge_label = nullptr;

lv_obj_t *setup_Charge() {
    if (lvgl_port_lock(5)) {
        if (Charge) {
            return nullptr;
        }
        Charge = lv_obj_create(lv_scr_act());
        lv_obj_add_event_cb(Charge, +[](lv_event_t *e) {
            destoryChargeUI();
        }, LV_EVENT_PRESSED, nullptr);
        lv_obj_set_size(Charge, LV_PCT(100), LV_PCT(100));
        lv_obj_set_scrollbar_mode(Charge, LV_SCROLLBAR_MODE_OFF);
        // Add style for Charge - LV_PART_MAIN | LV_STATE_DEFAULT
        lv_obj_set_style_bg_color(Charge, lv_color_hex(0x313131), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_grad_color(Charge, lv_color_hex(0x2195f6), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(Charge, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_grad_dir(Charge, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(Charge, lv_color_hex(0x2195f6), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_opa(Charge, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(Charge, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_radius(Charge, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        // Create Charge_bar_6ARDeqEe
        Charge_bar = lv_bar_create(Charge);
        lv_obj_set_size(Charge_bar, 194, 71);
        lv_obj_align(Charge_bar, LV_ALIGN_TOP_MID, 0, 62);
        lv_obj_set_scrollbar_mode(Charge_bar, LV_SCROLLBAR_MODE_OFF);
        lv_bar_set_value(Charge_bar, HardWare::batteryPercent, LV_ANIM_OFF);
        // Add style for bar_6ARDeqEe - LV_PART_MAIN | LV_STATE_DEFAULT
        lv_obj_set_style_bg_color(Charge_bar, lv_color_hex(0x2195f6), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_grad_color(Charge_bar, lv_color_hex(0x2195f6), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(Charge_bar, 60, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_grad_dir(Charge_bar, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_radius(Charge_bar, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_color(Charge_bar, lv_color_hex(0x2195f6), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_opa(Charge_bar, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_width(Charge_bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_spread(Charge_bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_ofs_x(Charge_bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_ofs_y(Charge_bar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        // Add style for bar_6ARDeqEe - LV_PART_INDICATOR | LV_STATE_DEFAULT
        lv_obj_set_style_bg_color(Charge_bar, lv_color_hex(0x2195f6), LV_PART_INDICATOR | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_grad_color(Charge_bar, lv_color_hex(0x2195f6),
                                       LV_PART_INDICATOR | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(Charge_bar, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_grad_dir(Charge_bar, LV_GRAD_DIR_NONE, LV_PART_INDICATOR | LV_STATE_DEFAULT);
        lv_obj_set_style_radius(Charge_bar, 10, LV_PART_INDICATOR | LV_STATE_DEFAULT);
        //Create Charge_label_f8JWocJk
        Charge_label = lv_label_create(Charge);
        lv_obj_align(Charge_label, LV_ALIGN_TOP_MID, 0, 142);
        lv_obj_set_size(Charge_label, 100, 39);
        lv_obj_set_scrollbar_mode(Charge_label, LV_SCROLLBAR_MODE_OFF);
        lv_label_set_text_fmt(Charge_label, "%d%%", (int) HardWare::batteryPercent);
        lv_label_set_long_mode(Charge_label, LV_LABEL_LONG_WRAP);
        // Add style for label_f8JWocJk - LV_PART_MAIN | LV_STATE_DEFAULT
        lv_obj_set_style_bg_color(Charge_label, lv_color_hex(0x2095f5), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_grad_color(Charge_label, lv_color_hex(0x2195f6), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(Charge_label, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_grad_dir(Charge_label, LV_GRAD_DIR_NONE, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_radius(Charge_label, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_top(Charge_label, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_bottom(Charge_label, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_left(Charge_label, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_right(Charge_label, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_color(Charge_label, lv_color_hex(0xf52020), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_opa(Charge_label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_width(Charge_label, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_spread(Charge_label, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_ofs_x(Charge_label, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_ofs_y(Charge_label, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(Charge_label, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_align(Charge_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_line_space(Charge_label, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_letter_space(Charge_label, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_add_event_cb(Charge, [](lv_event_t *e) {
            ESP_LOGI("CHARGE","Update");
            lv_bar_set_value(Charge_bar, HardWare::batteryPercent, LV_ANIM_OFF);
            lv_label_set_text_fmt(Charge_label, "%d%%", (int) HardWare::batteryPercent);
        }, (lv_event_code_t)Gui::UPDATE_BAT, nullptr);
        lvgl_port_unlock();
        return Charge;
    }
    return NULL;
}

void destoryChargeUI() {
    if (lvgl_port_lock(5)) {
        if (Charge) {
            lv_obj_del(Charge);
            Charge = NULL;
        }
        lvgl_port_unlock();
    }

}