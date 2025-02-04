#pragma once
#include <stdint.h>
#include "driver/spi_master.h"

class RFID
{
private:
    spi_device_handle_t m_Handler;
public:
    bool Init(spi_host_device_t host, uint8_t miso, uint8_t mosi, uint8_t clk, uint8_t cs);
};