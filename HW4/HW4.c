#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "ssd1306.h"
#include "font.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 16
#define I2C_SCL 17
#define HEARTBEAT_LED 25

int main()
{
    stdio_init_all();

    // I2C Initialisation
    i2c_init(I2C_PORT, 100 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Heartbeat LED
    gpio_init(HEARTBEAT_LED);
    gpio_set_dir(HEARTBEAT_LED, GPIO_OUT);

    bool heartbeat_state = false;
    absolute_time_t last_blink = get_absolute_time();

    // ADC0 setup: ADC0 is GPIO26
    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);

    // OLED setup
    ssd1306_setup();
    ssd1306_clear();
    ssd1306_update();

    // Making space for the messages
    char message1[50];
    char message2[50];

    while (true)
    {
        // Read ADC0 and convert to volts
        uint16_t raw_adc = adc_read();
        float voltage = raw_adc * 3.3f / 4095.0f;

        // Clear display memory
        ssd1306_clear();

        // Blink a pixel using same heartbeat state
        ssd1306_drawPixel(0, 0, heartbeat_state);

        // Write ADC voltage
        sprintf(message1, "ADC0 = %.2f V", voltage);
        drawMessage(0, 10, message1);

        // FPS
        unsigned int before_update = to_us_since_boot(get_absolute_time());
        ssd1306_update();
        unsigned int after_update = to_us_since_boot(get_absolute_time());

        float fps = 1000000.0f / (after_update - before_update);

        ssd1306_clear();
        ssd1306_drawPixel(0, 0, heartbeat_state);

        sprintf(message1, "ADC0 = %.2f V", voltage);
        sprintf(message2, "FPS = %.1f", fps);

        drawMessage(0, 10, message1);
        drawMessage(0, 24, message2);

        ssd1306_update();

        sleep_ms(10);

        // Heartbeat LED
        gpio_put(HEARTBEAT_LED, 1);
        sleep_ms(500);

        gpio_put(HEARTBEAT_LED, 0);
        sleep_ms(500);
    }
}

void drawChar(int x, int y, char c)
{
    if (c < 0x20 || c > 0x7F)
    {
        c = '?';
    }

    int index = c - 0x20;

    for (int col = 0; col < 5; col++)
    {
        unsigned char column = ASCII[index][col];

        for (int row = 0; row < 8; row++)
        {
            unsigned char pixel = (column >> row) & 0x01;
            ssd1306_drawPixel(x + col, y + row, pixel);
        }
    }
}

void drawMessage(int x, int y, char *message)
{
    int cursor_x = x;

    while (*message != '\0')
    {
        drawChar(cursor_x, y, *message);
        cursor_x += 6; // 5 pixels for letter + 1 pixel space
        message++;
    }
}
