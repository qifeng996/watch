//
// Created by Wind on 2025/3/21.
//
#include <esp_log.h>
#include <MsgController.h>

#include "Gui.h"
#include "audio.h"

void Gui::set_up_wifi_info(lv_obj_t* parent)
{
    wifi_info_container = lv_obj_create(parent);
    lv_obj_remove_style_all(wifi_info_container);
    //    lv_obj_set_style_radius(wifi_info_container, 0, 0);
    lv_obj_center(wifi_info_container);
    lv_obj_set_size(wifi_info_container, LV_PCT(95), LV_PCT(95));
    lv_obj_set_scrollbar_mode(wifi_info_container, LV_SCROLLBAR_MODE_OFF);
    wifi_state_img = lv_image_create(wifi_info_container);
    lv_obj_add_flag(wifi_state_img, LV_OBJ_FLAG_IGNORE_LAYOUT);
    LV_IMG_DECLARE(wifi)
    lv_image_set_src(wifi_state_img, &wifi);
    lv_obj_align(wifi_state_img, LV_ALIGN_TOP_RIGHT, 0, 0);
    ap_toggle_button = lv_button_create(wifi_info_container);
    lv_obj_set_size(ap_toggle_button, 100, 90);
    lv_obj_align(ap_toggle_button, LV_ALIGN_TOP_LEFT, 5, 0);
    lv_obj_t* btn_label = lv_label_create(ap_toggle_button);
    lv_obj_center(btn_label);
    lv_obj_set_style_text_align(btn_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(btn_label, "AP");
    lv_obj_set_style_text_color(btn_label, lv_color_white(), 0);
    lv_obj_add_style(btn_label, &font_28_style, 0);
    //    lv_label_set_text(ap_toggle_button, "AP");

    lv_obj_add_flag(ap_toggle_button, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_add_event_cb(ap_toggle_button, [](lv_event_t* e)
    {
        // const audio_player_state_t state = audio_player_get_state();
        // if (state==AUDIO_PLAYER_STATE_PAUSE)
        // {
        //     audio->resume();
        // }else if (state==AUDIO_PLAYER_STATE_PLAYING)
        // {
        //     audio->pause();
        // }else if(state==AUDIO_PLAYER_STATE_IDLE)
        // {
        //     audio->play();
        // }
        // gui->set_up_alert();
        msgController->push_msg(new Msg(Msg::Alter));
    }, LV_EVENT_VALUE_CHANGED, nullptr);
    lv_obj_t* info_box = lv_obj_create(wifi_info_container);
    lv_obj_set_size(info_box, LV_PCT(95), LV_PCT(57));
    lv_obj_align(info_box, LV_ALIGN_BOTTOM_MID, 0, 0);
}
