#pragma once
#include <stdint.h>
#include "driver/spi_master.h"
#include "MFRC522.h"

class RFID
{
private:
    spi_device_handle_t m_Handler;
public:
    bool Init(spi_host_device_t host, uint8_t miso, uint8_t mosi, uint8_t clk, uint8_t cs, uint8_t rst);
    bool IsNewCardPresent() { return PICC_IsNewCardPresent(m_Handler); }
    bool Select(Uid* uid) { return PICC_Select(m_Handler, uid, 0) == STATUS_OK; };
};