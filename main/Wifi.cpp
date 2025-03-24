//
// Created by Wind on 2025/3/18.
//

#include <esp_wifi.h>
#include <cstring>
#include <esp_log.h>
#include "Wifi.h"
#include "MsgController.h"
#include "lwip/udp.h"
#define MAX_RETRY 5
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1
static int s_retry_num = 0;
static EventGroupHandle_t s_wifi_event_group;
udp_pcb* udp_pcb;
Wifi::Wifi() = default;
Wifi::~Wifi() = default;

void udp_recv_cb(void* arg, struct udp_pcb* pcb, struct pbuf* p,
                 const ip_addr_t* addr, u16_t port)
{
    ESP_LOGI("UDP", "RECVED");
    msgController->push_msg(new Msg(Msg::Alter));
    pbuf_free(p);
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        s_retry_num = 0;
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < MAX_RETRY)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI("WIFI", "Retrying to connect to the WiFi...");
        }
        else
        {
            msgController->push_msg(new Msg_DisConnect());
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        auto* event = (ip_event_got_ip_t*)event_data;
        ESP_LOGI("WIFI", "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        msgController->push_msg(new Msg_WifiConnect("192.168.9.1"));
        s_retry_num = 0;
        udp_init();
        udp_pcb = udp_new();
        udp_bind(udp_pcb,IP_ADDR_ANY, 8080);
        udp_recv(udp_pcb, udp_recv_cb, nullptr);
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}


esp_err_t Wifi::wifi_init()
{
    s_wifi_event_group = xEventGroupCreate();
    esp_netif_init();
    esp_event_loop_create_default();

    esp_netif_create_default_wifi_ap();
    esp_netif_create_default_wifi_sta();
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, nullptr, nullptr);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, nullptr, nullptr);
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    wifi_config_t ap_config = {
        .ap = {
            .ssid = "ESP_AP",
            .password = "12345678",
            .authmode = WIFI_AUTH_WPA2_PSK,
            .ssid_hidden = true,
            .max_connection = 4,
        },
    };

    esp_wifi_set_mode(WIFI_MODE_APSTA);
    esp_wifi_set_config(WIFI_IF_AP, &ap_config);
    esp_wifi_start();

    ESP_LOGI("WIFI", "ESP32 in AP mode, SSID: %s, Password: %s", "ESP_AP", "12345678");

    return 0;
}

static auto html_form = R"(
  <html>
    <body>
      <h1>WiFi 配置</h1>
      <form action="/set_wifi" method="POST">
        <label for="ssid">WiFi SSID:</label><br>
        <input type="text" id="ssid" name="ssid"><br>
        <label for="password">WiFi 密码:</label><br>
        <input type="password" id="password" name="password"><br><br>
        <input type="submit" value="提交">
      </form>
    </body>
  </html>
)";

esp_err_t handle_root(httpd_req_t* req)
{
    httpd_resp_send(req, html_form, static_cast<ssize_t>(strlen(html_form)));
    return ESP_OK;
}

esp_err_t handle_set_wifi(httpd_req_t* req)
{
    char content[1024];
    if (const int ret = httpd_req_recv(req, content, req->content_len); ret <= 0)
    {
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
    else
    {
        content[ret] = '\0'; // 确保字符串以 `\0` 结尾
        ESP_LOGI("HTTP", "Received Data: %s", content);
    }
    // 从 HTTP 表单中获取 SSID 和密码
    char ssid[32] = {0};
    char password[64] = {0};

    if (httpd_query_key_value(content, "ssid", ssid, sizeof(ssid)) == ESP_OK)
    {
        ESP_LOGI("WIFI", "SSID: %s", ssid);
    }
    else
    {
        ESP_LOGE("WIFI", "Failed to get SSID");
    }

    if (httpd_query_key_value(content, "password", password, sizeof(password)) == ESP_OK)
    {
        ESP_LOGI("WIFI", "Password: %s", password);
    }
    else
    {
        ESP_LOGE("WIFI", "Failed to get Password");
    }
    wifi_mode_t mode;
    esp_wifi_get_mode(&mode);
    if (mode != WIFI_MODE_APSTA)
    {
        esp_wifi_set_mode(WIFI_MODE_APSTA);
    }
    esp_wifi_disconnect();

    // 等待连接断开完成

    // **配置 WiFi STA 并尝试连接**
    wifi_config_t wifi_config = {};
    strncpy((char*)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char*)wifi_config.sta.password, password, sizeof(wifi_config.sta.password) - 1);
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();
    esp_wifi_connect();
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           15000 / portTICK_PERIOD_MS); // 15 秒超时
    if (bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI("WIFI", "WiFi Connected!");
        httpd_resp_send(req, "WiFi 连接成功！设备已联网", HTTPD_RESP_USE_STRLEN);
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        ESP_LOGE("WIFI", "WiFi Connection Failed");
        httpd_resp_send(req, "WiFi 连接失败，请检查 SSID 和密码", HTTPD_RESP_USE_STRLEN);
        return ESP_FAIL;
    }
    else
    {
        ESP_LOGE("WIFI", "WiFi Connection Timeout");
        httpd_resp_send(req, "WiFi 连接超时，请重试", HTTPD_RESP_USE_STRLEN);
        return ESP_FAIL;
    }

    // **成功连接后，延迟几秒再关闭 AP，确保手机收到 HTTP 响应**
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    ESP_LOGI("WIFI", "Disabling AP mode...");
    // **等待 WiFi 连接结果**
    return ESP_OK;
}

httpd_handle_t start_webserver()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard; // 允许 URI 通配符匹配
    httpd_handle_t server = nullptr;
    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_uri_t uri_root = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = handle_root,
            .user_ctx = nullptr,
        };
        httpd_register_uri_handler(server, &uri_root);

        httpd_uri_t uri_set_wifi = {
            .uri = "/set_wifi",
            .method = HTTP_POST,
            .handler = handle_set_wifi,
            .user_ctx = nullptr,
        };
        httpd_register_uri_handler(server, &uri_set_wifi);
    }

    return server;
}
