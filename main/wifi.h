#pragma once

#include "freertos/FreeRTOS.h"
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
public:
    static bool Init(const char *ssid, const char *psswd, esp_event_handler_t handler);
};