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
#define HEARTBEAT_LED 

int main()
{
    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    
    // Heartbeat LED
    gpio_init(HEARTBEAT_LED);
    gpio_set_dir(HEARTBEAT_LED, GPIO_OUT);

    bool heartbeat_state = false;
    absolute_time_t last_blink = get_absolute_time();

    while (true) {
        // Heartbeat blink every 500 ms (copied over from HW3)
        if (absolute_time_diff_us(last_blink, get_absolute_time()) > 500000) {
            heartbeat_state = !heartbeat_state;
            gpio_put(HEARTBEAT_LED, heartbeat_state);
            last_blink = get_absolute_time();
        }

        sleep_ms(1);
    }
}

void drawChar(int x, int y, char c) {
    if (c < 0x20 || c > 0x7F) {
        c = '?';
    }

    int index = c - 0x20;

    for (int col = 0; col < 5; col++) {
        unsigned char column = ASCII[index][col];

        for (int row = 0; row < 8; row++) {
            unsigned char pixel = (column >> row) & 0x01;
            ssd1306_drawPixel(x + col, y + row, pixel);
        }
    }
}

void drawString(int x, int y, char *string) {
    int cursor_x = x;

    while (*string != '\0') {
        drawChar(cursor_x, y, *string);
        cursor_x += 6; // 5 pixels for letter + 1 pixel space
        string++;
    }
}

