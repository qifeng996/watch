//
// Created by Wind on 2025/3/21.
//
#include "Gui.h"

void Gui::set_up_bat_info(lv_obj_t *parent) {
    bat_info_container = lv_obj_create(parent);
    lv_obj_center(bat_info_container);
    lv_obj_set_size(bat_info_container, LV_PCT(95), LV_PCT(95));
    lv_obj_set_scrollbar_mode(bat_info_container, LV_SCROLLBAR_MODE_OFF);
    bat_percent_bar = lv_bar_create(bat_info_container);
    lv_obj_set_size(bat_percent_bar, 180, 50);
    lv_obj_align(bat_percent_bar, LV_ALIGN_TOP_MID, 0, 60);
    lv_obj_set_style_radius(bat_percent_bar, 10, LV_PART_MAIN);
    lv_obj_set_style_radius(bat_percent_bar, 10, LV_PART_INDICATOR);
    lv_bar_set_value(bat_percent_bar, 50, LV_ANIM_ON);

    bat_percent_label = lv_label_create(bat_info_container);
    lv_obj_set_size(bat_percent_label, LV_PCT(95), 30);
    lv_obj_add_style(bat_percent_label, &font_28_style, LV_PART_MAIN);
    lv_obj_align(bat_percent_label, LV_ALIGN_BOTTOM_LEFT, 0, -50);
    lv_label_set_text(bat_percent_label, "Percent:50%");
    bat_state_label = lv_label_create(bat_info_container);
    lv_obj_set_size(bat_state_label, LV_PCT(95), 30);
    lv_obj_add_style(bat_state_label, &font_28_style, LV_PART_MAIN);
    lv_obj_align(bat_state_label, LV_ALIGN_BOTTOM_LEFT, 0, -25);
    lv_label_set_text(bat_state_label, "State:Charg");
}


