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

bool timer_interrupt_function(_unused struct repeating_timer *t)
{
    // Read the adc
    uint16_t result1 = adc_read();
    // Print Voltage
    printf("%f\r\n", (float)result1/4095*3.3);
}

int main()
{
    stdio_init_all();

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
