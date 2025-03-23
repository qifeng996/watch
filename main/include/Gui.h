//
// Created by Wind on 2025/3/18.
//

#ifndef WATCH_GUI_H
#define WATCH_GUI_H

#include "lvgl.h"
#include "charge.h"

LV_IMAGE_DECLARE(code);
LV_IMAGE_DECLARE(wifi);
LV_IMAGE_DECLARE(diswifi);

class Gui {
public:
    Gui();

    ~Gui();

    static uint32_t UPDATE_BAT;

    void main_page();

    lv_style_t font_28_style{};

    //--------------------wifi_info------------------------
    void set_up_wifi_info(lv_obj_t *parent);

    lv_obj_t *wifi_info_container = nullptr;
    lv_obj_t *wifi_state_img = nullptr;
    lv_obj_t *wifi_state_label = nullptr;
    lv_obj_t *wifi_ipaddr_label = nullptr;
    lv_obj_t *ap_toggle_button = nullptr;

    //--------------------bat_info--------------------------
    void set_up_bat_info(lv_obj_t *parent);

    lv_obj_t *bat_info_container = nullptr;
    lv_obj_t *bat_percent_bar = nullptr;
    lv_obj_t *bat_percent_label = nullptr;
    lv_obj_t *bat_state_label = nullptr;
private:
    enum disp_size_t {
        DISP_SMALL,
        DISP_MEDIUM,
        DISP_LARGE,
    };
    lv_disp_t *disp_handle{};
    disp_size_t disp_size{};
    const lv_font_t *font_large{};
    const lv_font_t *font_normal{};
    lv_obj_t *tv{};
    lv_obj_t *calendar{};
    lv_style_t style_text_muted{};
    lv_style_t style_title{};
    lv_style_t font_qingniao{};
    lv_obj_t *list{};
};

extern Gui *gui;
#endif //WATCH_GUI_H
