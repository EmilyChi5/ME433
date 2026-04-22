/* 
    HW2 - PWM Servo Sweep
    Code in C
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/timer.h"

// Functions
void setServo(int angle);

// Defining Global Constants/Variables
#define SERVO_PIN 16
#define PWM_FREQ 50.0
#define MIN_DUTY 0.015f   // 1.5%
#define MAX_DUTY 0.12f    // 12%

// PWM setup for 50 Hz
// 125 MHz / 125 = 1 MHz counter clock
// 1 MHz / 20000 = 50 Hz
#define PWM_CLKDIV 125.0f
#define PWM_WRAP 19999

bool timer_interrupt_function(__unused struct repeating_timer *t) 
{
    // Read the adc
    uint16_t result1 = adc_read();
    // Print Voltage
    printf("ADC Voltage: %f\r\n", (float)result1/4095*3.3);
    return true;
}

int main()
{
    stdio_init_all();

    // Using Timer Interrupt
    struct repeating_timer timer;
    add_repeating_timer_ms(-100, timer_interrupt_function, NULL, &timer);

    // Turn on PWM
    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM); // Set LED Pin to be PWM
    uint slice_num = pwm_gpio_to_slice_num(SERVO_PIN); // Gets PWM Slice Number

    pwm_set_clkdiv(slice_num, PWM_CLKDIV); // Setting clock speed

    // Set PWM Frequency and resolution
    pwm_set_wrap(slice_num, PWM_WRAP);
    pwm_set_enabled(slice_num, true); // Turn on the PWM

    setServo(90);

    // Turning on adc
    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);

    while (true) {
        // Loop through servo angles
        int i = 0;

        // 0 to 180
        for (i = 0; i <= 180; i++){
            setServo(i);
            sleep_ms(15);
        }

        // 180 to 0
        for (i = 180; i >= 0; i--){
            setServo(i);
            sleep_ms(15);
        }

        sleep_ms(500);
    }
}

void setServo (int angle)
{
    // From Example Code: pwm_set_gpio_level(SERVO_PIN, (int)(((0.5)+(angle/180.0)*0.05)*6000));

    // Restrict Angle
    if (angle < 0)
        angle = 0;
    if (angle > 180)
        angle = 180;

    // Map angle 0->180 to duty cycle MIN_DUTY->MAX_DUTY
    float duty = MIN_DUTY + ((float)angle / 180.0f) * (MAX_DUTY - MIN_DUTY);

    // Since wrap = 19999, full scale is (PWM_WRAP + 1) counts
    uint16_t level = (uint16_t)(duty * (PWM_WRAP + 1)); // Convert duty cycle to PWM level

    pwm_set_gpio_level(SERVO_PIN, level);
} 