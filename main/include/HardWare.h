//
// Created by Wind on 2025/3/17.
//

#ifndef WATCH_HARDWARE_H
#define WATCH_HARDWARE_H

#include "XPowersLib.h"
#include "SensorDRV2605.hpp"
#include "esp_lvgl_port.h"


class HardWare {
public:
    HardWare();

    ~HardWare();

    void PMU_init();

    static void PMU_irq(void *arg);

    void Drv2605_init();

    void LCD_init();

    void setBrightness(uint8_t value);

    void setBrightness(uint8_t value, int time);

    XPowersAXP2101 PMU;
    SensorDRV2605 Motor;
    enum PowerCtrlChannel {
        WATCH_POWER_DISPLAY_BL,
        WATCH_POWER_TOUCH_DISP,
        WATCH_POWER_RADIO,
        WATCH_POWER_DRV2605,
        WATCH_POWER_GPS
    };
    TaskHandle_t Irq_Task_Handle{};
    static RTC_DATA_ATTR uint32_t brightness;
    static uint32_t batteryPercent;

    void powerIoctl(enum PowerCtrlChannel ch, bool enable);

    bool standby_flag = false;
    bool usbPlugIn = false;

    static void Irq_Task(void *arg);

private:
    i2c_master_bus_handle_t Sensor_i2c_bus_handle = nullptr;
    i2c_master_bus_handle_t Touch_i2c_bus_handle = nullptr;
    spi_host_device_t LCD_spi_bus_handle;
    lv_disp_t *disp_handle{};
    lv_indev_t *lvgl_touch_indev = nullptr;
};

extern HardWare *hardWare;

#endif //WATCH_HARDWARE_H
