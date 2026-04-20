/* 
    HW2 - PWM Servo Sweep
    Code in C
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"

// Functions
void setServo(int angle)

// Defining Global Constants/Variables
#define SERVO_PIN 16
#define PWM_FREQ 50.0

bool timer_interrupt_function(__unused struct repeating_timer *t)
{
    // Read the adc
    uint16_t result1 = adc_read();
    // Print Voltage
    printf("%f\r\n", (float)result1/4095*3.3);
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

    // Divide by float from 1 to 255 until clock frequency is 150 MHz
    float div = 50; 
    pwm_set_clkdiv(slice_num, div); // Setting clock speed
    uint16_t wrap = 6000; // Has to be less than 65535

    // Set PWM Frequency and resolution
    pwn_set_wrap(slice_num, wrap);
    pwm_set_enabled(slice_num, true); // Turn on the PWM
    pwm_set_gpio_level(SERVO_PIN, 0) // Sets duty cycle to 50%

    // Turning on adc
    adc_init();
    adc_gpio_init(26)
    adc_select_input(0)

    while (true) {
        // Loop through servo angles
        int i = 0;

        for (i = 10; i < 170; i++){
            setServo(i);
            sleep_ms(10);
        }

        for (i = 170; i > 10; i--){
            setServo(i);
            sleep_ms(10);
        }

        sleep_ms(1000);
    }
}

void setServo (int angle){
    pwm_set_gpio_level(SERVO_PIN, (int)((0.5)+(angle/180.0)*0.05)*6000);
}