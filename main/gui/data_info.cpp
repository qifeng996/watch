//
// Created by Wind on 2025/5/16.
//
#include "Gui.h"

void create_gas_item(lv_obj_t *parent, lv_obj_t **p_label, lv_obj_t **p_progress, const char *gas_name) {
    static int32_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(2), LV_GRID_TEMPLATE_LAST};
    static int32_t row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    lv_obj_t *cont = lv_obj_create(parent);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
    lv_obj_set_grid_dsc_array(cont, col_dsc, row_dsc);

    lv_obj_set_style_pad_all(cont, 6, 0);
    lv_obj_set_style_pad_column(cont, 8, 0);
    lv_obj_set_style_border_width(cont, 1, 0);
    lv_obj_set_style_border_color(cont, lv_palette_main(LV_PALETTE_GREY), 0);
    lv_obj_set_style_radius(cont, 4, 0);
    lv_obj_set_style_bg_color(cont, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(cont, LV_OPA_COVER, 0);

    lv_obj_t *label_name = lv_label_create(cont);
    lv_label_set_text(label_name, gas_name);
    lv_obj_set_style_text_font(label_name, &lv_font_montserrat_22, 0);
    lv_obj_set_grid_cell(label_name, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 0, 2); // 占2行


    *p_label = lv_label_create(cont);
    lv_label_set_text(*p_label, "");
    lv_obj_set_style_text_font(*p_label, &lv_font_montserrat_14, 0);
    lv_obj_set_grid_cell(*p_label, LV_GRID_ALIGN_END, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1); // 横向右对齐，垂直居中


    *p_progress = lv_bar_create(cont);
    lv_bar_set_range(*p_progress, 0, 100);
    lv_bar_set_value(*p_progress, 50, LV_ANIM_ON);
    lv_obj_set_grid_cell(*p_progress, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_END, 1, 1);
    lv_obj_set_size(*p_progress, LV_PCT(100), 8);
    lv_obj_set_style_bg_color(*p_progress, lv_palette_lighten(LV_PALETTE_GREY, 3), LV_PART_MAIN);
    lv_obj_set_style_bg_color(*p_progress, lv_palette_main(LV_PALETTE_RED), LV_PART_INDICATOR);

}


void Gui::set_up_data_info(lv_obj_t *parent) {
    data_info_container = lv_obj_create(parent);
    lv_obj_set_size(data_info_container, LV_PCT(95), LV_PCT(95));
    lv_obj_add_flag(data_info_container, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
    lv_obj_center(data_info_container);
    lv_obj_set_flex_flow(data_info_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_grow(data_info_container, 1);
    lv_obj_add_flag(data_info_container, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);

    create_gas_item(data_info_container, &CH4_label, &CH4_progress, "CH4");
    create_gas_item(data_info_container, &CO_label, &CO_progress, "CO");
    create_gas_item(data_info_container, &H2S_label, &H2S_progress, "H2S");
    create_gas_item(data_info_container, &O2_label, &O2_progress, "O2");
    create_gas_item(data_info_container, &SO2_label, &SO2_progress, "SO2");
    create_gas_item(data_info_container, &NO2_label, &NO2_progress, "NO2");
    create_gas_item(data_info_container, &CO2_label, &CO2_progress, "CO2");
    create_gas_item(data_info_container, &NH3_label, &NH3_progress, "NH3");
    create_gas_item(data_info_container, &PM2_5_label, &PM2_5_progress, "PM2.5");
    create_gas_item(data_info_container, &PM10_label, &PM10_progress, "PM10");
    create_gas_item(data_info_container, &TEMP_label, &TEMP_progress, "TEMP");
    create_gas_item(data_info_container, &HUMB_label, &HUMB_progress, "HUMB");
}

void update_data(lv_obj_t *label, lv_obj_t *progress, float value, const char *unit, float upLimit, float downLimit) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%.1f%s", value, unit);
    lv_label_set_text(label, buf);
    int percent = 0;
    if (value <= downLimit) {
        percent = 0;
    } else if (value >= upLimit) {
        percent = 100;
    } else {
        percent = (int) (((value - downLimit) / (upLimit - downLimit)) * 100.0f);
    }
    lv_bar_set_value(progress, percent, LV_ANIM_ON);
    if (percent > 70) {
        lv_obj_set_style_bg_color(progress, lv_palette_main(LV_PALETTE_RED), LV_PART_INDICATOR); // 超上限：红
    } else if (percent < 30) {
        lv_obj_set_style_bg_color(progress, lv_palette_main(LV_PALETTE_BLUE), LV_PART_INDICATOR); // 低于下限：蓝
    } else {
        lv_obj_set_style_bg_color(progress, lv_palette_main(LV_PALETTE_GREEN), LV_PART_INDICATOR); // 正常：绿
    }
}

void Gui::set_data(Msg_DataUpdate::Data data) {
    if (lvgl_port_lock(5)) {
        update_data(this->CH4_label, this->CH4_progress, data.CH4, "%", 1000.f, 0.f);
        update_data(this->CO_label, this->CO_progress, data.CO, "ppm", 1000.f, 0.f);
        update_data(this->H2S_label, this->H2S_progress, data.H2S, "ppm", 1000.f, 0.f);
        update_data(this->O2_label, this->O2_progress, data.O2, "%", 100.f, 0.f);
        update_data(this->SO2_label, this->SO2_progress, data.SO2, "ppm", 1000.f, 0.f);
        update_data(this->NO2_label, this->NO2_progress, data.NO2, "ppm", 1000.f, 0.f);
        update_data(this->CO2_label, this->CO2_progress, data.CO2, "ppm", 1000.f, 0.f);
        update_data(this->NH3_label, this->NH3_progress, data.NH3, "ppm", 1000.f, 0.f);
        update_data(this->PM2_5_label, this->PM2_5_progress, data.PM2_5, "ug/m3", 400.f, 0.f);
        update_data(this->PM10_label, this->PM10_progress, data.PM10, "ug/m3", 400.f, 0.f);
        update_data(this->TEMP_label, this->TEMP_progress, data.TEMP, "°C", 80.f, -40);
        update_data(this->HUMB_label, this->HUMB_progress, data.HUMB, "%", 100.f, 0.f);
        lvgl_port_unlock();
    }
}