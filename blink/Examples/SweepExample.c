#include "servo.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <time.h>
#include "hardware/pwm.h"
#include "hardware/adc.h"

// servo : 0
// IR : 14
// RGB : 15
// buzzer  : 16
// Photo resistor : 16
#define SERVO_PIN 0
#define IR_PIN 14
#define RGB_PIN 2
#define BUZZER_PIN 18
#define PHOTO_RESISTOR_PIN 26

#define PWM_PERIOD 2000  // Set the PWM period to 2ms
// int onetime = 1;

void run_servo(int servoPin , int duration_in_seconds);
void run_buzzer(int buzzer_pin , int duration_in_seconds);
void run_buzzer_Infinitely(int buzzer_pin);
void initiate_IR(int IR_pin);
void set_led_brightness(uint slice_num, uint percent);
void play_with_LED(int sleep_value, uint slice_num);
void play_with_LED_with_photoresistance(int percent, uint slice_num);
void RGB_PWM_init(uint RGB_pin);
void setPWM_wrap(uint sliceNum, uint pwm_period);
void set_Photoresistor(uint photoresistor_pin);

  
int main(){

    stdio_init_all();
    initiate_IR(IR_PIN);
    
    RGB_PWM_init(RGB_PIN);

    gpio_set_function(RGB_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(RGB_PIN);

    // Set the PWM period
    setPWM_wrap(slice_num, PWM_PERIOD);

    set_Photoresistor(PHOTO_RESISTOR_PIN);

    int servoDuration = 5; 
    // run_servo(SERVO_PIN, servoDuration); 
    while (true){
            stdio_init_all();

        printf("IR: %d\n", gpio_get(IR_PIN));
        if(!gpio_get(IR_PIN)){
            run_buzzer_Infinitely(BUZZER_PIN);
            // if (onetime){
                run_servo(RIGHT_SERVO_PIN, servoDuration);
                // onetime = 0;
            // }
        }else{
            gpio_put(BUZZER_PIN, 0);
        }



        ////
        uint16_t raw_value = adc_read();
        float voltage = raw_value * 3.3f / (1 << 12);  // Convert raw value to voltage

        printf("Analog reading: %d, Voltage: %.2fV\n", raw_value, voltage);
        play_with_LED_with_photoresistance((int)(voltage/3.3*100.0), slice_num);
        printf("percent: %d\n", (int)(voltage/3.3*100.0));
        sleep_ms(1000);  // Delay for 1 second

}


// run_buzzer_Infinitely(BUZZER_PIN);
    // int buzzerDuration = 2; 
    // run_buzzer(BUZZER_PIN, buzzerDuration);

    // initiate_IR(IR_PIN);
    // while (true) {
    //     if(gpio_get(IR_PIN)){
    //         run_buzzer(BUZZER_PIN, 2);
    //     }

    // }


}

void run_servo(int servoPin , int duration_in_seconds)
{
    int currentMillis = 400;    
    setServo(servoPin, currentMillis);
    bool direction = true;

    time_t start_time = time(NULL); // get the current time
    time_t end_time = start_time + duration_in_seconds; // calculate the end time

    while (time(NULL) < end_time)
    {
        currentMillis += (direction)?5:-5;
        if (currentMillis >= 2400) direction = false;
        if (currentMillis <= 400) direction = true;
        setMillis(servoPin, currentMillis);
        sleep_ms(10);
    }
}

void run_buzzer(int buzzer_pin , int duration_in_seconds){

    time_t start_time = time(NULL); // get the current time
    time_t end_time = start_time + duration_in_seconds; // calculate the end time
    printf("start time: %ld\n", start_time);
    printf("end time: %ld\n", end_time);
    while (time(NULL) < end_time) {
        printf("time: %ld\n", time(NULL));
        run_buzzer_Infinitely(buzzer_pin);
    }
    gpio_put(buzzer_pin, 0);

}
void run_buzzer_Infinitely(int buzzer_pin){

    gpio_init(buzzer_pin);
    gpio_set_dir(buzzer_pin, GPIO_OUT);
    gpio_put(buzzer_pin, 1);

}

void initiate_IR(int IR_pin){
    gpio_init(IR_pin);
    gpio_set_dir(IR_pin, GPIO_IN);
}


///////////////////////////////////////////////

//#include "hardware/irq.h"




void set_led_brightness(uint slice_num, uint percent) {
    // Ensure the percent value is between 0 and 100
    percent = (percent > 100) ? 100 : (percent < 0) ? 0 : percent;

    // Convert the percent value to a duty cycle
    uint duty_cycle = (uint)((percent / 100.0f) * PWM_PERIOD);

    // Set the PWM duty cycle for the LED
    pwm_set_chan_level(slice_num, PWM_CHAN_A, duty_cycle);
}

void play_with_LED(int sleep_value, uint slice_num) {
    int increment = 1; 
    for (int percent = 1; percent <= 100; percent += increment) {
        set_led_brightness(slice_num, percent);
        sleep_ms(10);

        increment = (percent == 100) ? -1 : (percent == 0) ? 1 : increment;

        if (percent == 0) break;
    }
    sleep_ms(sleep_value);
}


void play_with_LED_with_photoresistance(int percent, uint slice_num) {
    set_led_brightness(slice_num, percent);
    sleep_ms(10);
}

void RGB_PWM_init(uint RGB_pin){
    gpio_init(RGB_pin);
    gpio_set_dir(RGB_pin, GPIO_FUNC_PWM);
}

void setPWM_wrap(uint sliceNum, uint pwm_period){
    pwm_set_wrap(sliceNum, pwm_period);
    pwm_set_enabled(sliceNum, true);
}
void set_Photoresistor(uint photoresistor_pin){
    adc_init();
    adc_gpio_init(photoresistor_pin);
    adc_select_input(0);
}
