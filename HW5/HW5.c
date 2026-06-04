#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "ssd1306hw5.h"
#include "fonthw5.h"

// New functions
void mpu_write(unsigned char reg, unsigned char value);
unsigned char mpu_read(unsigned char reg);
void mpu6050_init(void);

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SCL 17
#define I2C_SDA 16
#define HEARTBEAT_LED 25

// Registers for IMU
#define MPU_ADDR 0x68

// config registers
#define CONFIG 0x1A
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define PWR_MGMT_1 0x6B
#define PWR_MGMT_2 0x6C
// sensor data registers:
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H   0x41
#define TEMP_OUT_L   0x42
#define GYRO_XOUT_H  0x43
#define GYRO_XOUT_L  0x44
#define GYRO_YOUT_H  0x45
#define GYRO_YOUT_L  0x46
#define GYRO_ZOUT_H  0x47
#define GYRO_ZOUT_L  0x48
#define WHO_AM_I     0x75


int main()
{
    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 100*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Heartbeat LED
    gpio_init(HEARTBEAT_LED);
    gpio_set_dir(HEARTBEAT_LED, GPIO_OUT);
    
    // Checking WHO_AM_I register
    unsigned char who = mpu_read(WHO_AM_I);

    if (who != 0x68 && who != 0x98) {
        while (true) {
            gpio_put(HEARTBEAT_LED, 1);
            sleep_ms(100);
            gpio_put(HEARTBEAT_LED, 0);
            sleep_ms(100);
        }
    }

    mpu6050_init();

    while (true) {
        printf("MPU6050 connected!\n");

        gpio_put(HEARTBEAT_LED, 1);
        sleep_ms(500);
        gpio_put(HEARTBEAT_LED, 0);
        sleep_ms(500);
    }
}

// Copied over drawing functions from last HW
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

void mpu_write(unsigned char reg, unsigned char value) {
    unsigned char buf[2];
    buf[0] = reg;
    buf[1] = value;
    i2c_write_blocking(i2c0, MPU_ADDR, buf, 2, false);
}

// Read IMU
unsigned char mpu_read(unsigned char reg) {
    unsigned char value;
    i2c_write_blocking(i2c0, MPU_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c0, MPU_ADDR, &value, 1, false);
    return value;
}

// To initalize IMU
void mpu6050_init() {
    mpu_write(PWR_MGMT_1, 0x00);    
    mpu_write(ACCEL_CONFIG, 0x00);  // +/- 2g
    mpu_write(GYRO_CONFIG, 0x18);   // +/- 2000 dps
}