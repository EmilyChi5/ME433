// Standard Libraries
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 16 // This is physically pin 21
#define I2C_SCL 17

// Constants
#define HEARTBEAT_LED 25
#define MCP_ADDR 0x20

#define IODIR 0x00
#define GPIO 0x09
#define OLAT 0x0A

#define BUTTON_PIN 0 //GP00
#define LED_PIN 7 //GP07

void writeRegister(unsigned char address, unsigned char reg, unsigned char value);

unsigned char readRegister(unsigned char address, unsigned char reg);

void setMCPPin(unsigned char pin, bool value);

int main() {
    stdio_init_all();

    // Initialize Pico heartbeat LED
    gpio_init(HEARTBEAT_LED);
    gpio_set_dir(HEARTBEAT_LED, GPIO_OUT);

    // Initialize I2C
    i2c_init(I2C_PORT, 100 * 1000); // 100 kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);

    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    sleep_ms(100);

    // IODIR: 1 = input, 0 = output
    // GP7 output = bit 7 is 0
    // GP0 input = bit 0 is 1
    // All other pins inputs = bits 1-6 are 1
    // Binary: 01111111 = 0x7F
    writeRegister(MCP_ADDR, IODIR, 0x7F);

    // Start with MCP LED off
    writeRegister(MCP_ADDR, OLAT, 0x00);

    bool heartbeat_state = false;
    absolute_time_t last_blink = get_absolute_time();

    while (true) {
        // Heartbeat blink every 500 ms
        if (absolute_time_diff_us(last_blink, get_absolute_time()) > 500000) {
            heartbeat_state = !heartbeat_state;
            gpio_put(HEARTBEAT_LED, heartbeat_state);
            last_blink = get_absolute_time();
        }

        // Read MCP GPIO register
        unsigned char gpio_value = readRegister(MCP_ADDR, GPIO);

        // Button is connected from GP0 to GND with pull-up.
        // Not pressed = 1, pressed = 0.
        bool button_pressed = ((gpio_value & (1 << BUTTON_PIN)) == 0);

        if (button_pressed) {
            setMCPPin(LED_PIN, true);
        } else {
            setMCPPin(LED_PIN, false);
        }

        sleep_ms(1);
    }
}

// All my functions
void writeRegister(unsigned char address, unsigned char reg, unsigned char value) {
    unsigned char buf[2];
        buf[0] = reg;
        buf[1] = value;

        i2c_write_blocking(I2C_PORT, address, buf, 2, false);
}

unsigned char readRegister(unsigned char address, unsigned char reg) {
    unsigned char value;

    i2c_write_blocking(I2C_PORT, address, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, address, &value, 1, false);

    return value;
}

void setMCPPin(unsigned char pin, bool value) {
    unsigned char current_olat = readRegister(MCP_ADDR, OLAT);

    if (value) {
        current_olat = current_olat | (1 << pin);   // set bit
    } else {
        current_olat = current_olat & ~(1 << pin);  // clear bit
    }

    writeRegister(MCP_ADDR, OLAT, current_olat);
}