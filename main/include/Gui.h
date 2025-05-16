//
// Created by Wind on 2025/3/18.
//

#ifndef WATCH_GUI_H
#define WATCH_GUI_H


#include "esp_lvgl_port.h"
#include "MsgController.h"

LV_IMAGE_DECLARE(code);
LV_IMAGE_DECLARE(wifi);
LV_IMAGE_DECLARE(diswifi);
LV_IMAGE_DECLARE(danger);

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

    //--------------------data_info------------------------
    void set_up_data_info(lv_obj_t *parent);

    void set_data(Msg_DataUpdate::Data data);

    lv_obj_t *data_info_container = nullptr;
    lv_obj_t *CH4_label = nullptr;
    lv_obj_t *CO_label = nullptr;
    lv_obj_t *H2S_label = nullptr;
    lv_obj_t *O2_label = nullptr;
    lv_obj_t *SO2_label = nullptr;
    lv_obj_t *NO2_label = nullptr;
    lv_obj_t *CO2_label = nullptr;
    lv_obj_t *NH3_label = nullptr;
    lv_obj_t *PM2_5_label = nullptr;
    lv_obj_t *PM10_label = nullptr;
    lv_obj_t *TEMP_label = nullptr;
    lv_obj_t *HUMB_label = nullptr;
    lv_obj_t *CH4_progress = nullptr;
    lv_obj_t *CO_progress = nullptr;
    lv_obj_t *H2S_progress = nullptr;
    lv_obj_t *O2_progress = nullptr;
    lv_obj_t *SO2_progress = nullptr;
    lv_obj_t *NO2_progress = nullptr;
    lv_obj_t *CO2_progress = nullptr;
    lv_obj_t *NH3_progress = nullptr;
    lv_obj_t *PM2_5_progress = nullptr;
    lv_obj_t *PM10_progress = nullptr;
    lv_obj_t *TEMP_progress = nullptr;
    lv_obj_t *HUMB_progress = nullptr;

    //--------------------alert--------------------------
    void set_up_alert();

    void destroy_alert();

    lv_obj_t *alert_page = nullptr;
    lv_timer_t *alert_timer = nullptr;

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
