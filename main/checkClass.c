#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "MFRC522.h"
#include "freertos/task.h"
#include "esp_log.h"

/*
static const char* TAG = "RC522";
char username[16] = {0};
char password[16] = {0};
uint8_t card_rx_buffer[20];
uint8_t card_rx_len = 20;
MIFARE_Key key = 
{
 .keyByte = {0xff,0xff,0xff,0xff,0xff,0xff}
};
uint8_t req_buffer[16];
uint8_t req_len = 16;
uint8_t status = 0;
*/

bool PICC_DumpMifareClassicSector(spi_device_handle_t spi,Uid *uid,	MIFARE_Key *key, uint8_t sector) 
{
	uint8_t status;
	uint8_t firstBlock;		// Address of lowest address to dump actually last block dumped)
	uint8_t no_of_blocks;		// Number of blocks in sector
	bool isSectorTrailer;	// Set to true while handling the "last" (ie highest address) in the sector.

	// The access bits are stored in a peculiar fashion.
	// There are four groups:
	//		g[3]	Access bits for the sector trailer, block 3 (for sectors 0-31) or block 15 (for sectors 32-39)
	//		g[2]	Access bits for block 2 (for sectors 0-31) or blocks 10-14 (for sectors 32-39)
	//		g[1]	Access bits for block 1 (for sectors 0-31) or blocks 5-9 (for sectors 32-39)
	//		g[0]	Access bits for block 0 (for sectors 0-31) or blocks 0-4 (for sectors 32-39)
	// Each group has access bits [C1 C2 C3]. In this code C1 is MSB and C3 is LSB.
	// The four CX bits are stored together in a nible cx and an inverted nible cx_.
	uint8_t c1, c2, c3;		// Nibbles
	uint8_t c1_, c2_, c3_;		// Inverted nibbles
	bool invertedError;		// True if one of the inverted nibbles did not match
	uint8_t g[4];				// Access bits for each of the four groups.
	uint8_t group;				// 0-3 - active group for access bits
	bool firstInGroup;		// True for the first block dumped in the group

	// Determine position and size of sector.
	if (sector < 32) // Sectors 0..31 has 4 blocks each
    { 
		no_of_blocks = 4;
		firstBlock = sector * no_of_blocks;
	}
	else if (sector < 40) // Sectors 32-39 has 16 blocks each
    { 
		no_of_blocks = 16;
		firstBlock = 128 + (sector - 32) * no_of_blocks;
	}
	else // Illegal input, no MIFARE Classic PICC has more than 40 sectors.
		return false;

	// Dump blocks, highest address first.
	uint8_t byteCount;
	uint8_t buffer[18];
	uint8_t blockAddr;
	isSectorTrailer = true;
	invertedError = false;	// Avoid "unused variable" warning.
	for (int8_t blockOffset = no_of_blocks - 1; blockOffset >= 0; blockOffset--) 
    {
		blockAddr = firstBlock + blockOffset;
		if (isSectorTrailer) 
        {
			status = PCD_Authenticate(spi,PICC_CMD_MF_AUTH_KEY_A, firstBlock, key, uid);
			if (status != STATUS_OK) 
				return false;
		}
		// Read block
		byteCount = sizeof(buffer);
		status = MIFARE_Read(spi,blockAddr, buffer, &byteCount);
		if (status != STATUS_OK) 
			return false;

		// Parse sector trailer data
		if (isSectorTrailer) 
        {
			c1  = buffer[7] >> 4;
			c2  = buffer[8] & 0xF;
			c3  = buffer[8] >> 4;
			c1_ = buffer[6] & 0xF;
			c2_ = buffer[6] >> 4;
			c3_ = buffer[7] & 0xF;
			invertedError = (c1 != (~c1_ & 0xF)) || (c2 != (~c2_ & 0xF)) || (c3 != (~c3_ & 0xF));
			g[0] = ((c1 & 1) << 2) | ((c2 & 1) << 1) | ((c3 & 1) << 0);
			g[1] = ((c1 & 2) << 1) | ((c2 & 2) << 0) | ((c3 & 2) >> 1);
			g[2] = ((c1 & 4) << 0) | ((c2 & 4) >> 1) | ((c3 & 4) >> 2);
			g[3] = ((c1 & 8) >> 1) | ((c2 & 8) >> 2) | ((c3 & 8) >> 3);
			isSectorTrailer = false;
		}

		// Which access group is this block in?
		if (no_of_blocks == 4) 
        {
			group = blockOffset;
			firstInGroup = true;
		}
		else 
        {
			group = blockOffset / 5;
			firstInGroup = (group == 3) || (group != (blockOffset + 1) / 5);
		}
	}

	return true;
} // End PICC_DumpMifareClassicSectorToSerial()

bool PICC_DumpMifareClassic(spi_device_handle_t spi, Uid *uid, PICC_Type piccType, MIFARE_Key *key) 
{
	uint8_t no_of_sectors = 0;
	switch (piccType) {
		case PICC_TYPE_MIFARE_MINI:
			// Has 5 sectors * 4 blocks/sector * 16 bytes/block = 320 bytes.
			no_of_sectors = 5;
			break;

		case PICC_TYPE_MIFARE_1K:
			// Has 16 sectors * 4 blocks/sector * 16 bytes/block = 1024 bytes.
			no_of_sectors = 16;
			break;

		case PICC_TYPE_MIFARE_4K:
			// Has (32 sectors * 4 blocks/sector + 8 sectors * 16 blocks/sector) * 16 bytes/block = 4096 bytes.
			no_of_sectors = 40;
			break;

		default: // Should not happen. Ignore.
			break;
	}

	// Dump sectors, highest address first.
	if (no_of_sectors) 
    {
		for (int8_t i = no_of_sectors - 1; i >= 0; i--)
        {
			if (!PICC_DumpMifareClassicSector(spi,uid, key, i))
                return false;
        }
    }
	PICC_HaltA(spi); // Halt the PICC before stopping the encrypted session.
	PCD_StopCrypto1(spi);

    return true;
} // End PICC_DumpMifareClassicToSerial()

bool PICC_DumpMifareUltralight(spi_device_handle_t spi) 
{
    uint8_t status;
    uint8_t byteCount;
    uint8_t buffer[18];
    uint8_t i;

	// Try the mpages of the original Ultralight. Ultralight C has more pages.
	for (uint8_t page = 0; page < 16; page +=4) // Read returns data for 4 pages at a time.
    { 
		// Read pages
		byteCount = sizeof(buffer);
		status = MIFARE_Read(spi,page, buffer, &byteCount);
		if (status != STATUS_OK) 
        {
			printf("MIFARE_Read() failed: ");
            return false;
		}
	}

    return true;
} // End PICC_DumpMifareUltralightToSerial()

bool PICC_Dump(spi_device_handle_t spi,Uid *uid) {
	// UID
	MIFARE_Key key;

	PICC_Type piccType = PICC_GetType(uid->sak);

	switch (piccType) 
    {
			case PICC_TYPE_MIFARE_MINI:
			case PICC_TYPE_MIFARE_1K:
			case PICC_TYPE_MIFARE_4K:
				//printf("PICC_TYPE_MIFARE_4K\n");
				// All keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
				for (uint8_t i = 0; i < 6; i++)
					key.keyByte[i] = 0xFF;
				return PICC_DumpMifareClassic(spi,uid, piccType, &key);
				break;

			case PICC_TYPE_MIFARE_UL:
				//printf("PICC_TYPE_MIFARE_UL\n");
				return PICC_DumpMifareUltralight(spi);
				break;

			case PICC_TYPE_ISO_14443_4:
			case PICC_TYPE_MIFARE_DESFIRE:
			case PICC_TYPE_ISO_18092:
			case PICC_TYPE_MIFARE_PLUS:
			case PICC_TYPE_TNP3XXX:
				printf("Dumping memory contents not implemented for that PICC type.\n");
				break;
			case PICC_TYPE_UNKNOWN:
			case PICC_TYPE_NOT_COMPLETE:
			default:
				break; // No memory dump here
    }

    return false;
}

void app_main(void)
{
    esp_err_t ret;
    spi_device_handle_t spi;
    spi_bus_config_t buscfg={
        .miso_io_num= PIN_NUM_MISO,
        .mosi_io_num= PIN_NUM_MOSI,
        .sclk_io_num= PIN_NUM_CLK,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1
    };
    spi_device_interface_config_t devcfg={
        .clock_speed_hz=5000000,               //Clock out at 5 MHz
        .mode=0,                                //SPI mode 0
        .spics_io_num= PIN_NUM_CS,               //CS pin
        .queue_size=7,                          //We want to be able to queue 7 transactions at a time
        //.pre_cb=ili_spi_pre_transfer_callback,  //Specify pre-transfer callback to handle D/C line
    };
    //Initialize the SPI bus
    ret=spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    assert(ret==ESP_OK);
    //Attach the RFID to the SPI bus
    ret=spi_bus_add_device(SPI2_HOST, &devcfg, &spi);
    assert(ret==ESP_OK);
   
    PCD_Init(spi);

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
        if(PICC_IsNewCardPresent(spi))                   //Checking for new card
    	{
    		GetStatusCodeName(PICC_Select(spi,&uid,0));
    		if (PICC_Dump(spi,&uid))
            {
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