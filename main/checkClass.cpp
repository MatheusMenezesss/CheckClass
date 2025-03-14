#include <stdio.h>
#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_timer.h"
#include "esp_log.h"

#include "rfid.h"
#include "wifi.h"
#include "tcp_client.h"
#include "db.h"

#include "lcd.h"

#include <string>

#define SSID "CINGUESTS"
#define PASSWORD "acessocin"

#define UID 0xdfa7be4b

std::string display_err_msg;
uint32_t display_err_msg_idx = 0;

enum class STATE { IDDLE, ONLINE_MODE, OFFLINE_MODE, ERROR_RECOVERY };

void restart()
{
    for (int i = 5; i >= 0; i--) 
    {
        ESP_LOGW("Main", "Reiniciando em %ds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    fflush(stdout);
    esp_restart();
}

void accept_request(const gpio_num_t red_led, const gpio_num_t green_led, const gpio_num_t buzzer)
{
    gpio_set_level(red_led, false);
    gpio_set_level(green_led, true);
    gpio_set_level(buzzer, true);

    vTaskDelay(400 / portTICK_PERIOD_MS);

    gpio_set_level(red_led, true);
    gpio_set_level(green_led, false);
    gpio_set_level(buzzer, false);
}

void reject_request(const gpio_num_t buzzer)
{
    gpio_set_level(buzzer, true);

    vTaskDelay(700 / portTICK_PERIOD_MS);

    gpio_set_level(buzzer, false);
}

void display_err(const char *emsg)
{
    LCD::SetCursor(2, 0);
    LCD::Stringf(emsg);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

extern "C" void app_main(void)
{
    if (!WIFI::Init(SSID, PASSWORD))
    {
        ESP_LOGE("Main", "Nao foi possivel iniciar o sistema de wifi\n");
        restart();
    }

    if (!TCPClient::Init("172.22.67.253", 8080))
    {
        ESP_LOGE("Main", "Nao foi possivel iniciar o sistema de cliente TCP\n");
        restart();
    }

    if (!DB::Init(4))
    {
        ESP_LOGE("Main", "Nao foi possivel iniciar o sistema de banco de dados\n");
        restart();
    }

	if (!RFID::Init(SPI2_HOST, 19, 23, 18, 2, 13))
	{
		ESP_LOGE("Main", "Não foi possível iniciar o sistema rfid.\n");
		restart();
	}

    if (!LCD::Init(100 * 1000, 22, 21))
    {
		ESP_LOGE("Main", "Não foi possível iniciar o sistema LCD.\n");
		restart();
    }

    const gpio_num_t red_led = GPIO_NUM_5;
    const gpio_num_t green_led = GPIO_NUM_4;
    const gpio_num_t buzzer = GPIO_NUM_27;
    
    gpio_set_direction(red_led, GPIO_MODE_OUTPUT);
    gpio_set_direction(green_led, GPIO_MODE_OUTPUT);
    gpio_set_direction(buzzer, GPIO_MODE_OUTPUT);

    gpio_set_level(red_led, false);
    gpio_set_level(green_led, false);
    gpio_set_level(buzzer, false);

    TCPClient::Send("request_db");

    char buffer[1024];
    TCPClient::Receive(buffer, 1024);
    
    FILE *f = fopen("/db/example.txt", "wb");

    if (f != nullptr)
    {
        printf("Foi possível abrir o arquivo\n");

        fprintf(f, "%s", buffer);

        fclose(f);

        f = fopen("/db/example.txt", "r");
        if (f != nullptr)
        {
            char line[128];

            fgets(line, sizeof(line), f);
            fclose(f);

            // strip newline
            char *pos = strchr(line, '\n');
            if (pos)
                *pos = '\0';

            printf("Leu do arquivo: %s\n", line);
        }

    }

    STATE current_state = STATE::IDDLE;
    LCD::String("State: IDDLE");

    int64_t start = esp_timer_get_time();
    LCD::SetCursor(2, 0);
    display_err_msg = "STRING GRANDE PARA TESTE";
    LCD::String(display_err_msg.c_str());
    int64_t accum = 0;

    bool update_idx_msg = false;

    while(1)
    {
        int64_t end = esp_timer_get_time();
        int64_t delta_time = (end - start) / 1000.f;
        start = end;

        if (update_idx_msg && accum > 500)
        {
            display_err_msg_idx = (display_err_msg_idx + 1) % display_err_msg.size();
            LCD::String(display_err_msg.c_str() + display_err_msg_idx);

            if (display_err_msg_idx == 0)
                update_idx_msg = false;
        }
        else if (accum > 1000)
                update_idx_msg = true;

        /*
        switch (current_state)
        {
        case STATE::IDDLE:
        {
            Uid uid;
            if (RFID::IsNewCardPresent() && RFID::Select(&uid))
            {
                ESP_LOGI("Main", "Cartão detectado\n");
                uint32_t uid_converted;
                memcpy(&uid_converted, uid.uidByte, sizeof(uint32_t));
                if (uid_converted == UID)
                {
                    LCD::Clear();
                    LCD::String("State: ONLINE");
                    current_state = STATE::ONLINE_MODE;
                    gpio_set_level(red_led, true);
                }
                else
                {
                    LCD::SetCursor(2, 0);
                    LCD::Stringf("UID: %08x", uid_converted);
                }
            }
        }
            break;
        case STATE::ONLINE_MODE:
            if (RFID::IsNewCardPresent())
            {
                Uid alunoUid;
                if (RFID::Select(&aluno_uid))
                {
                    LCD::Clear();
                    LCD::String("State: ONLINE");

                    if (!TCPClient::SendUID(&aluno_uid))
                    {
                        reject_request(buzzer);

                        uint32_t err = TCPClient::GetErrorCode();
                        if (err != 0)
                        {
                            display_err("SERVER TIMEOUT");
                            LCD::Clear();
                            LCD::String("State: OFFLINE");
                            current_state = STATE::OFFLINE_MODE;
                        }
                        else
                            display_err("NAO CADASTRADO");
                    }
                    else
                        accept_request(red_led, green_led, buzzer);
                }
                else
                {
                    reject_request(buzzer);
                    display_err("REAPROXIMAR CARTAO");
                }
            }
            break;
        case STATE::OFFLINE_MODE:
        {

        }
            break;
        case STATE::ERROR_RECOVERY:
            break;
        default:
            ESP_LOGE("Main", "Esp entrou em um estado não reconhecido\n");
            restart();
            break;
        }
        */

    	vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}