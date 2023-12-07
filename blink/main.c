#include "servo.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <time.h>
#include "hardware/pwm.h"
#include "hardware/adc.h"
// #include "hardware/irq.h"
// #include "pico/multicore.h"

// Define GPIO pins for all components

#define RIGHT_SERVO_PIN 1
#define LEFT_SERVO_PIN 0
#define Entering_IR_PIN 20
#define Leaving_IR_PIN 19
#define BUZZER_PIN 28
#define PHOTO_RESISTOR_PIN 26
#define RGB_PIN 2

#define PWM_PERIOD 2000 // Set the PWM period to 2ms

// Function prototypes
void initiate_Entering_IR();
void initiate_Leaving_IR();
void initiate_Photoresistor();
void initiate_RGB(uint slice_num);
void initiate_Buzzer();
void set_led_brightness(uint slice_num, uint percent);
void play_with_LED_with_photoresistance(int percent, uint slice_num);
void open_gate();
void close_gate();

int main()
{
    // Initialize standard I/O
    stdio_init_all();

    // Initialize all components
    initiate_Entering_IR();
    initiate_Leaving_IR();
    initiate_Photoresistor();
    initiate_Buzzer();
    uint slice_num = pwm_gpio_to_slice_num(RGB_PIN);
    initiate_RGB(slice_num);

    // Gating system variables
    int crossing__start_time = 0;
    bool crossed_entering_IR = false;
    bool crossed_leaving_IR = false;
    close_gate(); // Ensure the gate is closed initially
    int iteration_number = 0;
    while (true)
    {
        // Check if entering IR sensor is triggered
        if (!gpio_get(Entering_IR_PIN))
        {
            if (!crossed_entering_IR) // this if statemeent is to prevent the unnecessary execution of the code if the entering IR is already crossed once
            {
                open_gate();
                crossed_entering_IR = true;
                crossing__start_time = time(NULL);
            }
        }
        // Check if leaving IR sensor is triggered
        if (!gpio_get(Leaving_IR_PIN))
        {
            crossed_leaving_IR = true;
        }
        else
        {
            if (crossed_leaving_IR) // Close the gate when leaving IR sensor is no longer triggered , after being triggered once

            {
                close_gate();
                crossed_entering_IR = false;
                crossed_leaving_IR = false;
                crossing__start_time = 0;
            }
        }
        // Activate the buzzer if someone stays in the gate for more than 10 seconds
        if (crossed_entering_IR && (time(NULL) - crossing__start_time > 10))
        {
            gpio_put(BUZZER_PIN, 1);
        }
        else
        {
            gpio_put(BUZZER_PIN, 0);
        }

        ///////////////////////// for varying the brightness of the LED according to the photoresistor
        uint16_t raw_value = adc_read();
        float voltage = raw_value * 3.3f / (1 << 12); // Convert raw value to voltage
        int light_intensity_percentage = (int)(voltage / 3.3 * 100.0);
        set_led_brightness(slice_num, light_intensity_percentage);

        //////////////////// printings
        printf("iteration number: %d\n", iteration_number);
        printf("RGB pin status: %d\n", gpio_get(RGB_PIN));
        printf("Analog reading: %d, Voltage: %.2fV\n", raw_value, voltage);
        printf("percent: %d\n", (int)(voltage / 3.3 * 100.0));
        printf("entering IR: %d\n", gpio_get(Entering_IR_PIN));
        printf("leaving IR: %d\n", gpio_get(Leaving_IR_PIN));
        printf("crossed entering IR: %d\n", crossed_entering_IR);
        printf("crossed leaving IR: %d\n", crossed_leaving_IR);
        printf("crossing start time: %d\n", crossing__start_time);
        printf("current time: %d\n", time(NULL));
        printf("\n");

        iteration_number++;
        sleep_ms(500);
    }
}

void initiate_Entering_IR()
{
    gpio_init(Entering_IR_PIN);
    gpio_set_dir(Entering_IR_PIN, GPIO_IN);
}
void initiate_Leaving_IR()
{
    gpio_init(Leaving_IR_PIN);
    gpio_set_dir(Leaving_IR_PIN, GPIO_IN);
}
void initiate_Photoresistor()
{
    adc_init();
    adc_gpio_init(PHOTO_RESISTOR_PIN);
    adc_select_input(0);
}
void initiate_RGB(uint slice_num)
{
    //
    gpio_init(RGB_PIN);
    gpio_set_dir(RGB_PIN, GPIO_FUNC_PWM);

    gpio_set_function(RGB_PIN, GPIO_FUNC_PWM);

    pwm_set_wrap(slice_num, PWM_PERIOD);
    pwm_set_enabled(slice_num, true);
}
void initiate_Buzzer()
{
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
}

void set_led_brightness(uint slice_num, uint percent)
{
    // Ensure the percent value is between 0 and 100
    percent = (percent > 100) ? 100 : (percent < 0) ? 0
                                                    : percent;

    // Convert the percent value to a duty cycle
    uint duty_cycle = (uint)((percent / 100.0f) * PWM_PERIOD);

    // Set the PWM duty cycle for the LED
    pwm_set_chan_level(slice_num, PWM_CHAN_A, duty_cycle);
}
void open_gate()
{
    // Pulse width for 90 degrees on the right servo
    int rightPulseWidth = 400; // Adjust this according to your servo's specifications

    // Pulse width for -90 degrees on the left servo
    int leftPulseWidth = 1400; // Adjust this according to your servo's specifications

    // Initialize the servos
    setServo(RIGHT_SERVO_PIN, rightPulseWidth);
    setServo(LEFT_SERVO_PIN, leftPulseWidth);

    // Set the servos to the desired positions
    setMillis(RIGHT_SERVO_PIN, rightPulseWidth);
    setMillis(LEFT_SERVO_PIN, leftPulseWidth);
}
void close_gate()
{
    // Pulse width for 90 degrees on the right servo
    int rightPulseWidth = 1250; // Adjust this according to your servo's specifications

    // Pulse width for -90 degrees on the left servo
    int leftPulseWidth = 100; // Adjust this according to your servo's specifications

    // Initialize the servos
    setServo(RIGHT_SERVO_PIN, rightPulseWidth);
    setServo(LEFT_SERVO_PIN, leftPulseWidth);

    // Set the servos to the desired positions
    setMillis(RIGHT_SERVO_PIN, rightPulseWidth);
    setMillis(LEFT_SERVO_PIN, leftPulseWidth);
}