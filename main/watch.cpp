#include <cstdio>
#include <freertos/FreeRTOS.h>
#include <nvs_flash.h>
#include "HardWare.h"
#include "Gui.h"
#include "Wifi.h"
#include "MsgController.h"

void Task(void *arg) {
    msgController = new Msg_Controller();
    hardWare = new HardWare();
    gui =new Gui();
    Wifi wifi = Wifi();
    hardWare->PMU_init();
    hardWare->Drv2605_init();
    hardWare->LCD_init();
    gui->main_page();
    wifi.wifi_init();
    start_webserver();
    ESP_LOGI("charge","send event1");
    while (true) {
        ESP_LOGI("charge","send event2");
        HardWare::batteryPercent = hardWare->PMU.getBatteryPercent();
        if (lvgl_port_lock(5)){
            lv_bar_set_value(gui->bat_percent_bar,HardWare::batteryPercent,LV_ANIM_ON);
            lv_label_set_text_fmt(gui->bat_percent_label,"Percent:%d%%",(int)HardWare::batteryPercent);
            lvgl_port_unlock();
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

extern "C" void app_main(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    xTaskCreate(Task, "task1", 4096, nullptr, 1, nullptr);
}
