#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "rfid.h"

extern "C" void app_main(void)
{
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