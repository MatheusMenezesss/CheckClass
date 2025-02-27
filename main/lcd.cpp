#include "lcd.h"
#include "esp_rom_sys.h"
#include <stdarg.h>

i2c_master_bus_handle_t LCD::s_BusHandle;
i2c_master_dev_handle_t LCD::s_DevHandle;

/* Quick helper function for single byte transfers */
void LCD::I2CWriteByte(uint8_t val)
{
    uint8_t buffer[1] = { val };
    i2c_master_transmit(s_DevHandle, buffer, sizeof(val), LCD_TIMEOUT_MS / portTICK_PERIOD_MS);
}

bool LCD::Init(uint32_t baudrate, uint8_t scl, uint8_t sda)
{
    i2c_master_bus_config_t buscfg = {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = (gpio_num_t)sda,
        .scl_io_num = (gpio_num_t)scl,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags = {
            .enable_internal_pullup = true,
        }
    };

    i2c_device_config_t devcfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = LCD_ADDR,
        .scl_speed_hz = baudrate,
    };

    ESP_ERROR_CHECK(i2c_new_master_bus(&buscfg, &s_BusHandle));

    ESP_ERROR_CHECK(i2c_master_bus_add_device(s_BusHandle, &devcfg, &s_DevHandle));

    SendByte(0x03, LCD_COMMAND);
    SendByte(0x03, LCD_COMMAND);
    SendByte(0x03, LCD_COMMAND);
    SendByte(0x02, LCD_COMMAND);

    SendByte(LCD_ENTRYMODESET | LCD_ENTRYLEFT, LCD_COMMAND);
    SendByte(LCD_FUNCTIONSET | LCD_2LINE, LCD_COMMAND);
    SendByte(LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSORON, LCD_COMMAND);
    Clear();

    return true;
}

// The display is sent a byte as two separate nibble transfers
void LCD::SendByte(uint8_t val, int mode) 
{
    uint8_t high = mode | (val & 0xF0) | LCD_BACKLIGHT;
    uint8_t low = mode | ((val << 4) & 0xF0) | LCD_BACKLIGHT;

    I2CWriteByte(high);
    ToggleEnable(high);
    I2CWriteByte(low);
    ToggleEnable(low);
}

void LCD::Clear(void) 
{
    SendByte(LCD_CLEARDISPLAY, LCD_COMMAND);
}

void LCD::ToggleEnable(uint8_t val) 
{
    // Toggle enable pin on LCD display
    // We cannot do this too quickly or things don't work
    esp_rom_delay_us(LCD_DELAY_US);
    I2CWriteByte(val | LCD_ENABLE_BIT);

    esp_rom_delay_us(LCD_DELAY_US);
    I2CWriteByte(val & ~LCD_ENABLE_BIT);
    
    esp_rom_delay_us(LCD_DELAY_US);
}

// go to location on LCD
void LCD::SetCursor(int line, int position) 
{
    int val = (line == 0) ? 0x80 + position : 0xC0 + position;
    SendByte(val, LCD_COMMAND);
}

void LCD::Char(char val) 
{
    SendByte(val, LCD_CHARACTER);
}

void LCD::String(const char *s) 
{
    while (*s)
        Char(*s++);
}

void LCD::Stringf(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    size_t size = vsnprintf(NULL, 0, format, args);
    if (size > 0)
    {
        char *buffer = (char*)malloc(size + 1);
        if (buffer != NULL)
        {
            vsnprintf(buffer, size + 1, format, args);
            String(buffer);
        }
    }

    va_end(args);
}
