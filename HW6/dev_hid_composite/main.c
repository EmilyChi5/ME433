/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hardware/i2c.h"
#include "hardware/gpio.h"

#include "bsp/board_api.h"
#include "tusb.h"

#include "usb_descriptors.h"

// DEFINES (Copied from previous HW)
#define MODE_BUTTON 15
#define MODE_LED 14

// New functions
void mpu_write(unsigned char reg, unsigned char value);
unsigned char mpu_read(unsigned char reg);
void mpu6050_init(void);
void mpu_read_all(short *ax, short *ay, short *az, short *temp, short *gx, short *gy, short *gz);

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SCL 17
#define I2C_SDA 16

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
#define TEMP_OUT_H 0x41
#define TEMP_OUT_L 0x42
#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x45
#define GYRO_YOUT_L 0x46
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48
#define WHO_AM_I 0x75

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+

/* Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum
{
  BLINK_NOT_MOUNTED = 250,
  BLINK_MOUNTED = 1000,
  BLINK_SUSPENDED = 2500,
};

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

void led_blinking_task(void);
void hid_task(void);

/*------------- MAIN -------------*/
int main(void)
{
  board_init();

  // Initalizing IMU and button stuff
  i2c_init(I2C_PORT, 100 * 1000);

  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA);
  gpio_pull_up(I2C_SCL);

  gpio_init(MODE_BUTTON);
  gpio_set_dir(MODE_BUTTON, GPIO_IN);
  gpio_pull_up(MODE_BUTTON);

  gpio_init(MODE_LED);
  gpio_set_dir(MODE_LED, GPIO_OUT);
  gpio_put(MODE_LED, 0);

  mpu6050_init();

  // init device stack on configured roothub port
  tud_init(BOARD_TUD_RHPORT);

  if (board_init_after_tusb)
  {
    board_init_after_tusb();
  }

  while (1)
  {
    tud_task(); // tinyusb device task
    led_blinking_task();

    hid_task();
  }
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
  blink_interval_ms = BLINK_MOUNTED;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
  blink_interval_ms = BLINK_NOT_MOUNTED;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void)remote_wakeup_en;
  blink_interval_ms = BLINK_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
  blink_interval_ms = tud_mounted() ? BLINK_MOUNTED : BLINK_NOT_MOUNTED;
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

static void send_hid_report(uint8_t report_id, uint32_t btn)
{
  // skip if hid is not ready yet
  if (!tud_hid_ready())
    return;

  switch (report_id)
  {
  case REPORT_ID_KEYBOARD:
  {
    // use to avoid send multiple consecutive zero report for keyboard
    static bool has_keyboard_key = false;

    if (btn)
    {
      uint8_t keycode[6] = {0};
      keycode[0] = HID_KEY_A;

      tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode);
      has_keyboard_key = true;
    }
    else
    {
      // send empty key report if previously has key pressed
      if (has_keyboard_key)
        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
      has_keyboard_key = false;
    }
  }
  break;

  case REPORT_ID_MOUSE:
  {
    static bool remote_mode = false;
    static bool last_button = true;
    static int circle_step = 0;

    bool button = gpio_get(MODE_BUTTON); // not pressed = 1, pressed = 0

    if (last_button == true && button == false)
    {
      remote_mode = !remote_mode;
      gpio_put(MODE_LED, remote_mode);
    }

    last_button = button;

    int8_t dx = 0;
    int8_t dy = 0;

    if (remote_mode)
    {
      // Circle
      circle_step++;

      if (circle_step < 25)
      {
        dx = 2;
        dy = 0;
      }
      else if (circle_step < 50)
      {
        dx = 0;
        dy = 2;
      }
      else if (circle_step < 75)
      {
        dx = -2;
        dy = 0;
      }
      else if (circle_step < 100)
      {
        dx = 0;
        dy = -2;
      }
      else
      {
        circle_step = 0;
      }
    }
    else
    {
      short ax, ay, az, temp, gx, gy, gz;

      mpu_read_all(&ax, &ay, &az, &temp, &gx, &gy, &gz);

      float ax_g = ax * 0.000061f;
      float ay_g = ay * 0.000061f;

      if (ax_g > 0.60)
        dx = 5;
      else if (ax_g > 0.35)
        dx = 3;
      else if (ax_g > 0.15)
        dx = 1;
      else if (ax_g < -0.60)
        dx = -5;
      else if (ax_g < -0.35)
        dx = -3;
      else if (ax_g < -0.15)
        dx = -1;

      if (ay_g > 0.60)
        dy = 5;
      else if (ay_g > 0.35)
        dy = 3;
      else if (ay_g > 0.15)
        dy = 1;
      else if (ay_g < -0.60)
        dy = -5;
      else if (ay_g < -0.35)
        dy = -3;
      else if (ay_g < -0.15)
        dy = -1;
    }

    // no button, right + down, no scroll, no pan
    tud_hid_mouse_report(REPORT_ID_MOUSE, 0x00, dx, dy, 0, 0);
  }
  break;

  case REPORT_ID_CONSUMER_CONTROL:
  {
    // use to avoid send multiple consecutive zero report
    static bool has_consumer_key = false;

    if (btn)
    {
      // volume down
      uint16_t volume_down = HID_USAGE_CONSUMER_VOLUME_DECREMENT;
      tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &volume_down, 2);
      has_consumer_key = true;
    }
    else
    {
      // send empty key report (release key) if previously has key pressed
      uint16_t empty_key = 0;
      if (has_consumer_key)
        tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &empty_key, 2);
      has_consumer_key = false;
    }
  }
  break;

  case REPORT_ID_GAMEPAD:
  {
    // use to avoid send multiple consecutive zero report for keyboard
    static bool has_gamepad_key = false;

    hid_gamepad_report_t report =
        {
            .x = 0, .y = 0, .z = 0, .rz = 0, .rx = 0, .ry = 0, .hat = 0, .buttons = 0};

    if (btn)
    {
      report.hat = GAMEPAD_HAT_UP;
      report.buttons = GAMEPAD_BUTTON_A;
      tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));

      has_gamepad_key = true;
    }
    else
    {
      report.hat = GAMEPAD_HAT_CENTERED;
      report.buttons = 0;
      if (has_gamepad_key)
        tud_hid_report(REPORT_ID_GAMEPAD, &report, sizeof(report));
      has_gamepad_key = false;
    }
  }
  break;

  default:
    break;
  }
}

// Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse etc ..)
// tud_hid_report_complete_cb() is used to send the next report after previous one is complete
void hid_task(void)
{
  // Poll every 10ms
  const uint32_t interval_ms = 10;
  static uint32_t start_ms = 0;

  if (board_millis() - start_ms < interval_ms)
    return; // not enough time
  start_ms += interval_ms;

  uint32_t const btn = board_button_read();

  // Remote wakeup
  if (tud_suspended() && btn)
  {
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    tud_remote_wakeup();
  }
  else
  {
    // Changed to mouse
    send_hid_report(REPORT_ID_MOUSE, btn);
  }
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report, uint16_t len)
{
  (void)instance;
  (void)len;

  uint8_t next_report_id = report[0] + 1u;

  if (next_report_id < REPORT_ID_COUNT)
  {
    send_hid_report(next_report_id, board_button_read());
  }
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen)
{
  // TODO not Implemented
  (void)instance;
  (void)report_id;
  (void)report_type;
  (void)buffer;
  (void)reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
{
  (void)instance;

  if (report_type == HID_REPORT_TYPE_OUTPUT)
  {
    // Set keyboard LED e.g Capslock, Numlock etc...
    if (report_id == REPORT_ID_KEYBOARD)
    {
      // bufsize should be (at least) 1
      if (bufsize < 1)
        return;

      uint8_t const kbd_leds = buffer[0];

      if (kbd_leds & KEYBOARD_LED_CAPSLOCK)
      {
        // Capslock On: disable blink, turn led on
        blink_interval_ms = 0;
        board_led_write(true);
      }
      else
      {
        // Caplocks Off: back to normal blink
        board_led_write(false);
        blink_interval_ms = BLINK_MOUNTED;
      }
    }
  }
}

//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+
void led_blinking_task(void)
{
  static uint32_t start_ms = 0;
  static bool led_state = false;

  // blink is disabled
  if (!blink_interval_ms)
    return;

  // Blink every interval ms
  if (board_millis() - start_ms < blink_interval_ms)
    return; // not enough time
  start_ms += blink_interval_ms;

  board_led_write(led_state);
  led_state = 1 - led_state; // toggle
}

// MPU Functions
void mpu_write(unsigned char reg, unsigned char value)
{
  unsigned char buf[2];
  buf[0] = reg;
  buf[1] = value;
  i2c_write_blocking(i2c0, MPU_ADDR, buf, 2, false);
}

// Read IMU
unsigned char mpu_read(unsigned char reg)
{
  unsigned char value;
  i2c_write_blocking(i2c0, MPU_ADDR, &reg, 1, true);
  i2c_read_blocking(i2c0, MPU_ADDR, &value, 1, false);
  return value;
}

// To initalize IMU
void mpu6050_init()
{
  mpu_write(PWR_MGMT_1, 0x00);
  mpu_write(ACCEL_CONFIG, 0x00); // +/- 2g
  mpu_write(GYRO_CONFIG, 0x18);  // +/- 2000 dps
}

// Nusrt Read
void mpu_read_all(short *ax, short *ay, short *az, short *temp, short *gx, short *gy, short *gz)
{
  unsigned char reg = ACCEL_XOUT_H;
  unsigned char data[14];

  i2c_write_blocking(i2c0, MPU_ADDR, &reg, 1, true);
  i2c_read_blocking(i2c0, MPU_ADDR, data, 14, false);

  *ax = (data[0] << 8) | data[1];
  *ay = (data[2] << 8) | data[3];
  *az = (data[4] << 8) | data[5];
  *temp = (data[6] << 8) | data[7];
  *gx = (data[8] << 8) | data[9];
  *gy = (data[10] << 8) | data[11];
  *gz = (data[12] << 8) | data[13];
}