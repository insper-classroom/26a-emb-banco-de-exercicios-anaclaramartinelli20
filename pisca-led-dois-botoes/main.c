#include <stdio.h>

#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "pico/stdlib.h"

const int BTN_PIN_G = 28;
const int BTN_PIN_Y = 26;

const int LED_PIN_G = 5;
const int LED_PIN_Y = 9;
const int LED_PIN_R = 13;

volatile int btn_g_f = 0; //flag do botão verde
volatile int btn_y_f = 0; // flag do botão amarelo

volatile int g_timer_g = 0;
volatile int g_timer_y = 0;
volatile int g_timer_r = 0;
volatile int g_fired_g = 0;
volatile int g_fired_y = 0;


void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) {  // fall edge
        if (gpio == btn_g_f){
            btn_g_f = 1;
        } else if (gpio == btn_y_f){
            btn_y_f = 1;
        }
    } 
}

bool timer_g_callback(repeating_timer_t *rt) {
    g_timer_g = 1;
    return true;  // keep repeating
}

bool timer_y_callback(repeating_timer_t *rt) {
    g_timer_y = 1;
    return true;  // keep repeating
}

bool timer_r_callback(repeating_timer_t *rt) {
    g_timer_r = 1;
    return true;  // keep repeating
}

int64_t alarm_g_callback(alarm_id_t id, void *user_data) {
    g_fired_g = 1;
    return 0;
}

int64_t alarm_y_callback(alarm_id_t id, void *user_data) {
    g_fired_y = 1;
    return 0;
}

int main() {
    stdio_init_all();

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);
    gpio_set_irq_enabled_with_callback(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);

    gpio_init(BTN_PIN_Y);
    gpio_set_dir(BTN_PIN_Y, GPIO_IN);
    gpio_pull_up(BTN_PIN_Y);
    gpio_set_irq_enabled_with_callback(BTN_PIN_Y, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    gpio_init(LED_PIN_Y);
    gpio_set_dir(LED_PIN_Y, GPIO_OUT);

    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    repeating_timer_t timer_g;
    add_repeating_timer_ms(100, timer_g_callback, NULL, &timer_g);

    repeating_timer_t timer_y;
    add_repeating_timer_ms(250, timer_y_callback, NULL, &timer_y);

    int led_g = 0;
    int led_y = 0;

    int alarm_enable_g = 0;
    int alarm_enable_y = 0;

    alarm_id_t alarm_g = -1;
    alarm_id_t alarm_y = -1;

    while (1) {
        if (g_timer_g && alarm_enable_g) {
            led_g = !led_g;
            gpio_put(LED_PIN_G, led_g);
            g_timer_g = 0;
        } else if (alarm_enable_g == 0) {
            gpio_put(LED_PIN_G, 0);
        }

        if (g_timer_y && alarm_enable_y) {
            led_y = !led_y;
            gpio_put(LED_PIN_Y, led_y);
            g_timer_y = 0;
        } else if (alarm_enable_y == 0) {
            gpio_put(LED_PIN_Y, 0);
        }

        if (btn_g_f == BTN_PIN_G) {
            if (alarm_enable_g == 0) {
                alarm_g = add_alarm_in_ms(1000, alarm_g_callback, NULL, false);
                alarm_enable_g = 1;
            }
            btn_g_f = 0;
        }

        if (btn_y_f == BTN_PIN_Y) {
            if (alarm_enable_y == 0) {
                alarm_y = add_alarm_in_ms(2000, alarm_y_callback, NULL, false);
                alarm_enable_y = 1;
            }
            btn_y_f = 0;
        }

        if (g_fired_g == 1) { // se o tempo do led verde acabou
            g_fired_g = 0; // desativa a flag
            alarm_enable_g = 0;  
            gpio_put(LED_PIN_G, 0);  // apaga o led verde

            if(alarm_enable_y==1){  // verifica se o amarelo está rodando para cancelar
                alarm_enable_y = 0;
                cancel_alarm(alarm_y);
                gpio_put(LED_PIN_Y, 0);  // apaga o led verde
            }
        }

        if (g_fired_y == 1) {
            g_fired_y = 0;
            alarm_enable_y = 0;
            gpio_put(LED_PIN_Y, 0);

            if(alarm_enable_g==1){
                alarm_enable_g = 0;
                cancel_alarm(alarm_g);
                gpio_put(LED_PIN_G, 0);
            }
        }

    }

    return 0;
}
