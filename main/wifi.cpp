#include "wifi.h"
#include "string.h"

bool WIFI::Init(const char *ssid, const char *psswd, esp_event_handler_t handler)
{
    int ssidLen = strlen(ssid), psswdLen = strlen(psswd);
    if (ssidLen >= 32 || psswdLen >= 64)
    {
        printf("Nome e/ou Senha maiores que o limite permitido\n");
        return false;
    }

    nvs_flash_init();

    // Inicia a interface de internet do ESP   
    esp_netif_init();
    // Inicia o event loop padrão do ESP
    esp_event_loop_create_default();
    // Cria a estação de wifi no driver do wifi
    esp_netif_create_default_wifi_sta();

    // Prepara a estação com as configurações padrões
    wifi_init_config_t wificfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wificfg);

    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, handler, NULL, NULL);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, handler, NULL, NULL);

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
    esp_wifi_set_mode(WIFI_MODE_STA);
    // Setando a configuração wi-fi
    esp_wifi_set_config(WIFI_IF_STA, &sta_cfg);
    // Rodando o wi-fi driver
    esp_wifi_start();

    return true;
}
