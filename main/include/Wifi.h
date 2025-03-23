//
// Created by Wind on 2025/3/18.
//

#ifndef WATCH_WIFI_H
#define WATCH_WIFI_H


#include <esp_err.h>
#include <esp_http_server.h>

httpd_handle_t start_webserver();

class Wifi {
public:
    Wifi();

    ~Wifi();

    esp_err_t wifi_init();


};


#endif //WATCH_WIFI_H
