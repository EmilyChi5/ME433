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

int main()
{
    stdio_init_all();

    // Initalizing SPI Pins
    spi_init(spi_default, 1000 * 1000); // the baud, or bits per second
    gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_TX, GPIO_FUNC_SPI);

    cs_select(PIN_CS);
    spi_write_blocking(SPI_PORT, data, len); // where data is a uint8_t array with length len
    cs_deselect(PIN_CS);

    float v[100];
    for (i = 0; i < 100; i++) {
        v[i] = sine(i)
    }

    while (true)
    {
        // call writeDAC
        float t = 0;
        t = t + .01;
        float voltage = (sine(2*PI*2*t)+1)/2*3.3
        writeDAC(channel, voltage)
        sleep_ms(10);
    }
}

void writeDAC(int channel, float v){

    uint8_t data[2];

    data[0] = 0b01110000;

    data[0] = data[0] | ((channel&0b1)<<7) // put the channel bit in

    uint16_t myV = v/3.3*1023;

    data[0] = data[0] | ((myV >> 6)&0b00001111)

    data[1] = (myV<<2)& 0xFF //0b11111100

    data[1] = 0b11111100;

    cs_select(PIN_CS);
    spi_write_blocking(SPI_PORT, data, len); // where data is a uint8_t array with length len
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