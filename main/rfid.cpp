#include "rfid.h"
#include "MFRC522.h"

bool RFID::Init(spi_host_device_t host, uint8_t miso, uint8_t mosi, uint8_t clk, uint8_t cs)
{
    spi_bus_config_t buscfg = {
        .mosi_io_num= mosi,
        .miso_io_num= miso,
        .sclk_io_num= clk,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1
    };

    spi_device_interface_config_t devcfg = {
        .mode=0,                                    //SPI mode 0
        .clock_speed_hz=5000000,                    //Clock out at 5 MHz
        .spics_io_num= cs,                          //CS pin
        .queue_size=7,                              //We want to be able to queue 7 transactions at a time
        //.pre_cb=ili_spi_pre_transfer_callback,    //Specify pre-transfer callback to handle D/C line
    };

    // Initialize the SPI bus
    // Attach the RFID to the SPI bus
    if (spi_bus_initialize(host, &buscfg, SPI_DMA_CH_AUTO) == ESP_OK && spi_bus_add_device(host, &devcfg, &m_Handler) == ESP_OK)
    {
        PCD_Init(m_Handler);
        return true;
    }

    return false;
}
