#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "nvs_flash.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_netif_types.h"
#include "esp_event.h"

class WIFI
{
private:
    static EventGroupHandle_t s_WifiEventGroup;

    static void Handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
public:
    static bool Init(const char *ssid, const char *psswd, esp_event_handler_t handler);
    static bool IsConnected();
};