#include <stdio.h>
#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "rfid.h"
#include "fatfs.h"
#include "wifi.h"

#define SSID "CINGUESTS"
#define PASSWORD "acessocin"

extern "C" void app_main(void)
{
    if (!WIFI::Init(SSID, PASSWORD, &event_handler))
    {
        printf("Nao foi possivel iniciar o sistema de wifi\n");
    }

    if (!FatFs::Init(4))
    {
        printf("Nao foi possivel iniciar o sistema de arquivos fat\n");
    }

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

    FILE *f = fopen("/db/example.txt", "wb");

    if (f != nullptr)
    {
        printf("Foi possível abrir o arquivo\n");

        fprintf(f, "Hello World!\n");

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