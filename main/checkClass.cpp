#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "rfid.h"

#include "freertos/event_groups.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_netif_types.h"

#define SSID "CINGUESTS"
#define PASSWORD "acessocin"

void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
        esp_wifi_connect();
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
        esp_wifi_connect();
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) 
    {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        printf("got ip: " IPSTR, IP2STR(&event->ip_info.ip));
    }
}

extern "C" void app_main(void)
{
    nvs_flash_init();
    
    esp_netif_init();
    esp_event_loop_create_default();
    
    wifi_init_config_t wifi_cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_cfg);

    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, NULL);

    esp_netif_create_default_wifi_sta();
    
    wifi_config_t sta_cfg = {
        .sta {
            .ssid = SSID,
            .password = PASSWORD,
            .scan_method = WIFI_ALL_CHANNEL_SCAN,
            .sort_method = WIFI_CONNECT_AP_BY_SIGNAL,
        }
    };

    sta_cfg.sta.threshold.rssi = -127;
    sta_cfg.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    sta_cfg.sta.threshold.rssi_5g_adjustment = 0;
    
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &sta_cfg);
    esp_wifi_start();
    esp_wifi_connect();

	RFID rfid;
	if (!rfid.Init(SPI2_HOST, 19, 23, 18, 2, 22))
	{
		printf("Não foi possível iniciar o sistema rfid.\n");

		for (int i = 5; i >= 0; i--) 
		{
            printf("Reiniciando em %ds...\n", i);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }

        fflush(stdout);
        esp_restart();
	}

    const gpio_num_t red_led = GPIO_NUM_5;
    const gpio_num_t green_led = GPIO_NUM_4;
    const gpio_num_t buzzer = GPIO_NUM_21;

    gpio_set_direction(red_led, GPIO_MODE_OUTPUT);
    gpio_set_direction(green_led, GPIO_MODE_OUTPUT);
    gpio_set_direction(buzzer, GPIO_MODE_OUTPUT);

    gpio_set_level(red_led, true);
    gpio_set_level(green_led, false);
    gpio_set_level(buzzer, false);

    while(1)
    {
		if (rfid.IsNewCardPresent())
		{
			Uid uid;
			if (rfid.Select(&uid))
			{
				printf("Uid: ");
				for(uint8_t i = 0; i < uid.size; i++)
					printf("%02x ", uid.uidByte[i]);
				printf("\n");

				gpio_set_level(red_led, false);
                gpio_set_level(green_led, true);
                gpio_set_level(buzzer, true);

                vTaskDelay(400 / portTICK_PERIOD_MS);

                gpio_set_level(red_led, true);
                gpio_set_level(green_led, false);
                gpio_set_level(buzzer, false);
			}
			else
			{
				gpio_set_level(buzzer, true);

                vTaskDelay(700 / portTICK_PERIOD_MS);

                gpio_set_level(buzzer, false);
			}
		}

    	vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}