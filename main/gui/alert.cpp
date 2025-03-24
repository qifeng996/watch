//
// Created by wind on 25-3-23.
//
#include <audio_player.h>
#include <functional>
#include <HardWare.h>
#include <misc/lv_timer_private.h>

#include "Gui.h"
#include "audio.h"
bool flag = false;

void alert_timer_cb(lv_timer_t* timer)
{
    hardWare->Motor.run();
    if (flag)
    {
        hardWare->setBrightness(240, 500);
    }
    else
    {
        hardWare->setBrightness(20, 500);
    }
    flag = !flag;
}

void Gui::set_up_alert()
{
    ESP_LOGI("GUI", "set_up_alert() called");
    if (alert_page)
    {
        return;
    }
    if (lvgl_port_lock(portMAX_DELAY))
    {
        hardWare->powerIoctl(HardWare::WATCH_POWER_DISPLAY_BL, true);
        if (hardWare->standby_flag)
        {
            lvgl_port_resume(); //恢复LVGL任务
            hardWare->standby_flag = false;
        }
        ESP_LOGI("GUI", "LVGL Lock Acquired");
        alert_page = lv_obj_create(lv_scr_act());
        lv_obj_add_event_cb(alert_page, [](lv_event_t* e)
        {
            gui->destroy_alert();
        }, LV_EVENT_PRESSED, nullptr);
        lv_obj_set_size(alert_page, LV_PCT(100), LV_PCT(100));
        lv_obj_set_scrollbar_mode(alert_page, LV_SCROLLBAR_MODE_OFF);
        lv_obj_t* danger_img = lv_image_create(alert_page);
        lv_obj_add_flag(danger_img, LV_OBJ_FLAG_IGNORE_LAYOUT);
        flag = true;
        lv_image_set_src(danger_img, &danger);
        lv_obj_center(danger_img);
        if (audio_player_get_state() == AUDIO_PLAYER_STATE_IDLE)
        {
            audio->play();
        }
        if (!alert_timer)
        {
            alert_timer = lv_timer_create(alert_timer_cb, 1500, this);
        }
        lvgl_port_unlock();
    }
}

void Gui::destroy_alert()
{
    if (lvgl_port_lock(5))
    {
        audio->stop();
        if (alert_timer)
        {
            lv_timer_del(alert_timer);
            alert_timer = nullptr;
        }
        if (alert_page)
        {
            lv_obj_del(alert_page);
            alert_page = nullptr;
        }
        hardWare->setBrightness(240);
        lvgl_port_unlock();
    }
}
