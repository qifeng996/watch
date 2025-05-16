//
// Created by Wind on 2025/3/19.
//

#ifndef WATCH_MSGCONTROLLER_H
#define WATCH_MSGCONTROLLER_H

#include <string>
#include <cstring>
#include <esp_log.h>
#include "freertos/FreeRTOS.h"

class Msg {
public:
    enum TYPE {
        Brightness,
        Motor,
        UsbPlugIn,
        UsbPlugOut,
        ChargeDone,
        ChargeStart,
        BatteryLow,
        WifiConnect,
        WifiDisConnect,
        PkeyLongPress,
        PkeyShortPress,
        Alter,
        DataUpdate
    };
    TYPE type;

    explicit Msg(TYPE _type) : type(_type) {

    }

    virtual ~Msg() = default;
};

class Brightness_Msg final : public Msg {
public:
    explicit Brightness_Msg(int _value) : Msg(Msg::Brightness), value(_value) {

    }

    int value = 0;

    ~Brightness_Msg() override = default;
};

class Msg_WifiConnect : public Msg {
public:
    explicit Msg_WifiConnect(const std::string &ipaddr) : Msg(Msg::WifiConnect), ip_addr(ipaddr) {}

    ~Msg_WifiConnect() override = default;

    std::string ip_addr;
};

class Msg_DisConnect : public Msg {


public:
    explicit Msg_DisConnect() : Msg(Msg::WifiDisConnect) {}

    ~Msg_DisConnect() override = default;

};

class Msg_DataUpdate : public Msg {
public:
    struct Data{
        float CH4;
        float CO;
        float H2S;
        float O2;
        float SO2;
        float NO2;
        float CO2;
        float NH3;
        float PM2_5;
        float PM10;
        float TEMP;
        float HUMB;
        uint8_t flag_CH4;
        uint8_t flag_CO;
        uint8_t flag_H2S;
        uint8_t flag_O2;
        uint8_t flag_SO2;
        uint8_t flag_NO2;
        uint8_t flag_CO2;
        uint8_t flag_NH3;
        uint8_t flag_PM2_5;
        uint8_t flag_PM10;
        uint8_t flag_TEMP;
        uint8_t flag_HUMB;
    } data;
    explicit Msg_DataUpdate(uint8_t *p_data) : Msg(Msg::DataUpdate) {
        if (p_data == nullptr) return;

        uint8_t data_count = p_data[2];  // 可用数据组数
        if (data_count > 12) data_count = 12;  // 防越界

        uint8_t *ptr = p_data + 3;  // 跳过前三个字段

        auto read_float = [](uint8_t *src) -> float {
            float f;
            uint32_t temp = (uint32_t)src[0] << 24 |
                            (uint32_t)src[1] << 16 |
                            (uint32_t)src[2] << 8  |
                            (uint32_t)src[3];
            memcpy(&f, &temp, sizeof(float));
            return f;
        };

        if (data_count > 0) { data.CH4 = read_float(ptr); data.flag_CH4 = ptr[4]; ptr += 5; }
        if (data_count > 1) { data.CO  = read_float(ptr); data.flag_CO  = ptr[4]; ptr += 5; }
        if (data_count > 2) { data.H2S = read_float(ptr); data.flag_H2S = ptr[4]; ptr += 5; }
        if (data_count > 3) { data.O2  = read_float(ptr); data.flag_O2  = ptr[4]; ptr += 5; }
        if (data_count > 4) { data.SO2 = read_float(ptr); data.flag_SO2 = ptr[4]; ptr += 5; }
        if (data_count > 5) { data.NO2 = read_float(ptr); data.flag_NO2 = ptr[4]; ptr += 5; }
        if (data_count > 6) { data.CO2 = read_float(ptr); data.flag_CO2 = ptr[4]; ptr += 5; }
        if (data_count > 7) { data.NH3 = read_float(ptr); data.flag_NH3 = ptr[4]; ptr += 5; }
        if (data_count > 8) { data.PM2_5 = read_float(ptr); data.flag_PM2_5 = ptr[4]; ptr += 5; }
        if (data_count > 9) { data.PM10  = read_float(ptr); data.flag_PM10  = ptr[4]; ptr += 5; }
        if (data_count > 10){ data.TEMP  = read_float(ptr); data.flag_TEMP  = ptr[4]; ptr += 5; }
        if (data_count > 11){ data.HUMB  = read_float(ptr); data.flag_HUMB  = ptr[4]; }
    }



    ~Msg_DataUpdate() override = default;

    bool isAlert() {
        ESP_LOGI("Sensor", "Checking alert flags and values:");

        auto log_flag = [](const char* name, float value, uint8_t flag) {
            // 打印浮点数和flag
            ESP_LOGI("Sensor", "%-6s: %-5s | %.3f", name, flag ? "TRUE" : "FALSE", value);

            // 打印浮点数的16进制字节值
            const uint8_t* p = reinterpret_cast<const uint8_t*>(&value);
            ESP_LOGI("Sensor", "%-6s hex bytes: %02X %02X %02X %02X", name, p[0], p[1], p[2], p[3]);
        };

        log_flag("CH4",   this->data.CH4,   this->data.flag_CH4);
        log_flag("CO",    this->data.CO,    this->data.flag_CO);
        log_flag("H2S",   this->data.H2S,   this->data.flag_H2S);
        log_flag("O2",    this->data.O2,    this->data.flag_O2);
        log_flag("SO2",   this->data.SO2,   this->data.flag_SO2);
        log_flag("NO2",   this->data.NO2,   this->data.flag_NO2);
        log_flag("CO2",   this->data.CO2,   this->data.flag_CO2);
        log_flag("NH3",   this->data.NH3,   this->data.flag_NH3);
        log_flag("PM2.5", this->data.PM2_5, this->data.flag_PM2_5);
        log_flag("PM10",  this->data.PM10,  this->data.flag_PM10);
        log_flag("TEMP",  this->data.TEMP,  this->data.flag_TEMP);
        log_flag("HUMB",  this->data.HUMB,  this->data.flag_HUMB);
        return this->data.flag_CH4 |
               this->data.flag_CO |
               this->data.flag_CO2 |
               this->data.flag_H2S |
               this->data.flag_HUMB |
               this->data.flag_NH3 |
               this->data.flag_NO2 |
               this->data.flag_O2 |
               this->data.flag_PM10 |
               this->data.flag_PM2_5 |
               this->data.flag_SO2 |
               this->data.flag_TEMP;
    }
};

class Msg_Controller {
public:
    Msg_Controller();

    TaskHandle_t msg_task_handle{};
    QueueHandle_t msg_queue_handle;
    SemaphoreHandle_t mutex_handle;
    bool alerted = false;
    static void msg_task(void *arg);

    void push_msg(const Msg *msg) const;
};

extern Msg_Controller *msgController;
#endif //WATCH_MSGCONTROLLER_H
