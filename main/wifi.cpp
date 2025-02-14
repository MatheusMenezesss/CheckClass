#include "wifi.h"
#include "string.h"
#include "esp_log.h"

#define WIFI_TAG "WIFI"

#define WIFI_CONNECTED_BIT (1 << 0)
#define WIFI_FAIL_BIT (1 << 1)

EventGroupHandle_t WIFI::s_WifiEventGroup = nullptr;

bool WIFI::Init(const char *ssid, const char *psswd, esp_event_handler_t handler)
{
    int ssidLen = strlen(ssid), psswdLen = strlen(psswd);
    if (ssidLen >= 32 || psswdLen >= 64)
    {
        ESP_LOGE(WIFI_TAG, "Nome e/ou senha maiores que o limite permitido\n");
        return false;
    }
    
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) 
    {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Inicia a interface de internet do ESP   
    ESP_ERROR_CHECK(esp_netif_init());
    // Inicia o event loop padrão do ESP
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    
    // Cria a estação de wifi no driver do wifi
    esp_netif_create_default_wifi_sta();

    // Prepara a estação com as configurações padrões
    wifi_init_config_t wificfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wificfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, Handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, Handler, NULL, NULL));

    // Criando as configurações do driver wi-fi
    wifi_config_t sta_cfg = {
        .sta = {
            .ssid = {0}, // Nome da rede
            .password = {0},  // Senha da rede
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        }
    };

    memcpy(sta_cfg.sta.ssid, ssid, ssidLen);
    memcpy(sta_cfg.sta.password, psswd, psswdLen);

    sta_cfg.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK; // Modo de autenticação da rede

    // Setando para o controlador wi-fi operar no modo estação
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    // Setando a configuração wi-fi
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_cfg));
    // Rodando o wi-fi driver
    ESP_ERROR_CHECK(esp_wifi_start());

    s_WifiEventGroup = xEventGroupCreate();

    EventBits_t bits = xEventGroupWaitBits(
        s_WifiEventGroup, 
        WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
        pdFALSE,
        pdFALSE,
        pdMS_TO_TICKS(30000)
    );

    if (bits & WIFI_FAIL_BIT || (bits & WIFI_CONNECTED_BIT) == 0) 
    {
        ESP_LOGE(WIFI_TAG, "Falha ou Timeout na conexão!");
        return false;
    }

    ESP_LOGI(WIFI_TAG, "Conectado ao Wi-Fi!");
    vEventGroupDelete(s_WifiEventGroup);
    return true;
}

void WIFI::Handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        ESP_LOGI(WIFI_TAG, "Conectando ao AP...");
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        wifi_event_sta_disconnected_t* disconnected = (wifi_event_sta_disconnected_t*) event_data;

        // Trata falha de autenticação imediatamente
        if (disconnected->reason == WIFI_REASON_AUTH_FAIL)
            xEventGroupSetBits(s_WifiEventGroup, WIFI_FAIL_BIT);

        ESP_LOGI(WIFI_TAG, "Reconectando ao AP...");
        esp_wifi_connect();
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) 
    {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(WIFI_TAG, "got ip: " IPSTR "\n", IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(s_WifiEventGroup, WIFI_CONNECTED_BIT);
    }
}
