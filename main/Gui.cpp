//
// Created by Wind on 2025/3/18.
//

#include <esp_log.h>
#include "Gui.h"
#include "esp_lvgl_port.h"
#include "MsgController.h"
#include "HardWare.h"
Gui *gui = nullptr;
LV_IMG_DECLARE(esp_logo)
LV_IMG_DECLARE(esp_text)
static void slider_event_cb(lv_event_t * e)
{
    auto * slider = (lv_obj_t *)lv_event_get_target(e); // 获取触发事件的对象
    int32_t value = lv_slider_get_value(slider); // 获取滑块当前值
    ESP_LOGI("SLIDER","change brightness %ld",value);
    msgController->push_msg(new Brightness_Msg(value *2+40));
}

uint32_t Gui::UPDATE_BAT = lv_event_register_id();
Gui::Gui() {
    lv_init();
    lv_style_init(&font_28_style);
    lv_style_set_text_font(&font_28_style, &lv_font_montserrat_22);
}

Gui::~Gui() = default;

void Gui::main_page() {
    if (lvgl_port_lock(0)) {
        tv= lv_tileview_create(lv_screen_active());

        lv_obj_t *t1_0 = lv_tileview_add_tile(tv,0,0,(lv_dir_t)(LV_DIR_RIGHT|LV_DIR_BOTTOM));
        lv_obj_t *t1_1 = lv_tileview_add_tile(tv,0,1,(lv_dir_t)(LV_DIR_TOP));
        lv_obj_t *t2 = lv_tileview_add_tile(tv,1,0,LV_DIR_HOR);
        lv_obj_t *t3 = lv_tileview_add_tile(tv,2,0,LV_DIR_HOR);
        lv_obj_t *t4 = lv_tileview_add_tile(tv,3,0,LV_DIR_LEFT);
        lv_obj_set_size(t1_0, LV_PCT(100), LV_PCT(100));
        lv_obj_set_size(t1_1, LV_PCT(100), LV_PCT(100));
        lv_obj_set_size(t2, LV_PCT(100), LV_PCT(100));
        lv_obj_set_size(t3, LV_PCT(100), LV_PCT(100));
        lv_obj_set_size(t4, LV_PCT(100), LV_PCT(100));
        this->set_up_wifi_info(t1_0);
        lv_obj_t *logo = lv_image_create(t1_1);
        lv_obj_add_flag(logo, LV_OBJ_FLAG_IGNORE_LAYOUT);

        lv_image_set_src(logo, &code);
        lv_obj_center(logo);
        this->set_up_bat_info(t2);
        this->set_up_data_info(t3);
        lv_obj_t *setting = lv_obj_create(t4);
        lv_obj_set_height(setting, LV_PCT(100));
        lv_obj_set_flex_grow(setting, 1);
        lv_obj_t *lv_brightness = lv_slider_create(setting);
        lv_obj_set_width(lv_brightness, LV_PCT(80));
        lv_slider_set_value(lv_brightness,(int32_t)(HardWare::brightness-40)/2,LV_ANIM_ON);
        lv_obj_add_event_cb(lv_brightness,slider_event_cb,LV_EVENT_VALUE_CHANGED, nullptr);
        lvgl_port_unlock();
    }
}


