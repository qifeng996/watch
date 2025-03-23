//
// Created by Wind on 2025/3/19.
//

#include "MsgController.h"
#include "HardWare.h"
#include "Gui.h"

Msg_Controller *msgController;

Msg_Controller::Msg_Controller() {
    mutex_handle = xSemaphoreCreateMutex();
    this->msg_queue_handle = xQueueCreate(5, sizeof(Msg *));
    xTaskCreate(msg_task, "msg/task", 2048, this, 2, &this->msg_task_handle);
}

void Msg_Controller::msg_task(void *arg) {
    auto *self = static_cast<Msg_Controller *>(arg);
    while (true) {
        Msg *msg = nullptr;
        xQueueReceive(self->msg_queue_handle, &msg, portMAX_DELAY);
        switch (msg->type) {
            case Msg::Brightness:
                hardWare->setBrightness(dynamic_cast<Brightness_Msg *>(msg)->value);
                break;
            case Msg::Motor:
                hardWare->Motor.run();
                break;
            case Msg::UsbPlugIn: {

                break;
            }
            case Msg::UsbPlugOut:
                break;
            case Msg::ChargeDone:

                break;
            case Msg::ChargeStart:
                break;
            case Msg::BatteryLow:
                break;
            case Msg::WifiConnect:
                if (lvgl_port_lock(5)) {
                    lv_image_set_src(gui->wifi_state_img, &wifi);
                    lvgl_port_unlock();
                }
                break;
            case Msg::WifiDisConnect:
                if (lvgl_port_lock(5)) {
                    lv_image_set_src(gui->wifi_state_img, &diswifi);
                    lvgl_port_unlock();
                }
                break;
            case Msg::PkeyLongPress:
                break;
            case Msg::PkeyShortPress:
                if (hardWare->standby_flag) {
                    lvgl_port_resume();//恢复LVGL任务
                } else {
                    lvgl_port_stop();//挂起LVGL任务
                }
                hardWare->powerIoctl(HardWare::WATCH_POWER_DISPLAY_BL, hardWare->standby_flag);
                hardWare->standby_flag = !hardWare->standby_flag;
                break;
            case Msg::Alter:
                break;
        }
        delete msg;
    }
}

void Msg_Controller::push_msg(const Msg *msg) const {
    xSemaphoreTake(this->mutex_handle, portMAX_DELAY);
    xQueueSend(this->msg_queue_handle, &msg, 0);
    xSemaphoreGive(this->mutex_handle);
}
