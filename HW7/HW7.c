#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

// Constants
#define SPI_PORT spi0
#define PIN_TX 19
#define PIN_SCK 18
#define PIN_CS 17

#define PI 3.14159265359

static inline void cs_select(uint cs_pin);
static inline void cs_deselect(uint cs_pin);
void writeDAC(int channel, float V);

int main()
{
    stdio_init_all();

    // Initalizing SPI Pins
    spi_init(spi_default, 1000 * 1000); // the baud, or bits per second
    // gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI); (I don't think I need this)
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_TX, GPIO_FUNC_SPI);

    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);

    float t = 0.0f;


    while (true)
    {
        // Making waves ranging from 0V to 3.3V
        // Basic Sine Wave
        float sine_voltage = (sinf(2.0f * PI * 2.0f *t) + 1.0f) / 2.0f * 3.3f;

        float triangle_voltage;
        float phase = fmodf(t, 1.0f);

        if (phase < 0.5f) {
            triangle_voltage = phase * 2.0f *3.3f;
        } else {
            triangle_voltage = (1.0f - phase) * 2.0f *3.3f;
        }

        writeDAC(0, sine_voltage); // Channel A
        writeDAC(1, triangle_voltage); // Channel B

        sleep_ms(5); // 200 Hz
        t += 0.0005f;

    }
}

void writeDAC(int channel, float V){

    if (V < 0.0f) {
        V = 0.0f;
    }

    if (V > 3.3f) {
        V > 3.3f;
    }

    uint16_t myV = (uint16_t)(V / 3.3f * 1023.0f);

    uint16_t command = 0;

    command |= (channel & 0b1) << 15; // channel A=0, B=1
    command |= 0b111 << 12;           // buffer=1, gain=1x, active=1
    command |= myV << 2;              // 10-bit value

    uint8_t data[2];
    data[0] = command >> 8;
    data[1] = command & 0xFF;

    cs_select(PIN_CS);
    spi_write_blocking(SPI_PORT, data, 2); // where data is a uint8_t array with length len
    cs_deselect(PIN_CS);
}

static inline void cs_select(uint cs_pin)
{
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 0);
    asm volatile("nop \n nop \n nop"); // FIXME
}

static inline void cs_deselect(uint cs_pin)
{
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 1);
    asm volatile("nop \n nop \n nop"); // FIXME
}