// Base code copied over from HW7 assignment

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

// New for RAM
#define PIN_RX 16     // RAM SO pin
#define PIN_RAM_CS 20 // RAM chip select

// 23K256 commands
#define RAM_WRITE 0b00000010
#define RAM_READ 0b00000011
#define RAM_WRMR 0b00000001
#define RAM_SEQ_MODE 0b01000000

#define PI 3.14159265359

static inline void cs_select(uint cs_pin);
static inline void cs_deselect(uint cs_pin);

void writeDAC(int channel, float V);
void spi_ram_init(void);
void ram_write_bytes(uint16_t address, uint8_t *data, int len);
void ram_read_bytes(uint16_t address, uint8_t *data, int len);
void makeDACbytes(int channel, float V, uint8_t data[2]);

int main()
{
    stdio_init_all();
    sleep_ms(3000);

    // Initalizing SPI Pins
    spi_init(spi_default, 1000 * 1000); // the baud, or bits per second
    // gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI); (I don't think I need this)
    gpio_set_function(PIN_RX, GPIO_FUNC_SPI); // Added for RAM
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_TX, GPIO_FUNC_SPI);

    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);

    gpio_init(PIN_RAM_CS);
    gpio_set_dir(PIN_RAM_CS, GPIO_OUT);
    gpio_put(PIN_RAM_CS, 1);

    // Initialize RAM
    spi_ram_init();

    sleep_ms(100);

    // sin values
    for (int i = 0; i < 1000; i++)
    {
        float voltage = (sinf(2.0f * PI * i / 1000.0f) + 1.0f) / 2.0f * 3.3f;

        uint8_t dac_data[2];
        makeDACbytes(0, voltage, dac_data);

        ram_write_bytes(2 * i, dac_data, 2);
    }

    printf("Finished writing sine wave to RAM\r\n");

    // Shows some values to the serial monitor 
    int test_samples[] = {0, 250, 500, 750};

    for (int j = 0; j < 4; j++) {

        uint8_t dac_data[2];

        ram_read_bytes(2 * test_samples[j], dac_data, 2);

        uint16_t command =
            ((uint16_t)dac_data[0] << 8) |
            dac_data[1];

        uint16_t dac_value =
            (command >> 2) & 0x03FF;

        printf("Sample %d = %u\r\n",
            test_samples[j],
            dac_value);
    }

    while (true)
    {
        for (int i = 0; i < 1000; i++)
        {
            uint8_t dac_data[2];

            ram_read_bytes(2 * i, dac_data, 2);

            cs_select(PIN_CS);
            spi_write_blocking(SPI_PORT, dac_data, 2);
            cs_deselect(PIN_CS);

            sleep_ms(1);
        }
    }
}

void writeDAC(int channel, float V)
{

    if (V < 0.0f)
    {
        V = 0.0f;
    }

    if (V > 3.3f)
    {
        V = 3.3f;
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

void spi_ram_init(void)
{
    uint8_t data[2];

    data[0] = RAM_WRMR;
    data[1] = RAM_SEQ_MODE;

    cs_select(PIN_RAM_CS);
    spi_write_blocking(SPI_PORT, data, 2);
    cs_deselect(PIN_RAM_CS);
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

void ram_write_bytes(uint16_t address, uint8_t *data, int len)
{
    uint8_t header[3];

    header[0] = RAM_WRITE;
    header[1] = address >> 8;
    header[2] = address & 0xFF;

    cs_select(PIN_RAM_CS);
    spi_write_blocking(SPI_PORT, header, 3);
    spi_write_blocking(SPI_PORT, data, len);
    cs_deselect(PIN_RAM_CS);
}

void ram_read_bytes(uint16_t address, uint8_t *data, int len)
{
    uint8_t header[3];

    header[0] = RAM_READ;
    header[1] = address >> 8;
    header[2] = address & 0xFF;

    cs_select(PIN_RAM_CS);
    spi_write_blocking(SPI_PORT, header, 3);
    spi_read_blocking(SPI_PORT, 0, data, len);
    cs_deselect(PIN_RAM_CS);
}

void makeDACbytes(int channel, float V, uint8_t data[2])
{
    if (V < 0.0f)
    {
        V = 0.0f;
    }

    if (V > 3.3f)
    {
        V = 3.3f;
    }

    uint16_t myV = (uint16_t)(V / 3.3f * 1023.0f);

    uint16_t command = 0;
    command |= (channel & 0b1) << 15;
    command |= 0b111 << 12;
    command |= myV << 2;

    data[0] = command >> 8;
    data[1] = command & 0xFF;
}
