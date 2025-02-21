#pragma once
#include <stdint.h>
#include "driver/spi_master.h"
#include "MFRC522.h"

class RFID
{
private:
    static spi_device_handle_t s_Handler;
public:
    static bool Init(spi_host_device_t host, uint8_t miso, uint8_t mosi, uint8_t clk, uint8_t cs, uint8_t rst);
    static bool IsNewCardPresent() { return PICC_IsNewCardPresent(s_Handler); }
    static bool Select(Uid* uid) { return PICC_Select(s_Handler, uid, 0) == STATUS_OK; };
};