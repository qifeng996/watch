//
// Created by Wind on 2025/3/19.
//

#ifndef WATCH_MSGCONTROLLER_H
#define WATCH_MSGCONTROLLER_H

#include <string>
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
        Alter
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

class Msg_Controller {
public:
    Msg_Controller();

    TaskHandle_t msg_task_handle{};
    QueueHandle_t msg_queue_handle;
    SemaphoreHandle_t mutex_handle;

    static void msg_task(void *arg);

    void push_msg(const Msg *msg) const;
};

extern Msg_Controller *msgController;
#endif //WATCH_MSGCONTROLLER_H
