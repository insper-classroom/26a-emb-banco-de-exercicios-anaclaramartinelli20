/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
utilizar timer para piscar o led
Sempre que o botão vermelho for pressionado, o tempo de pisca do LED vermelho deverá ser atualizado.
O LED não deve piscar enquanto o botão estiver sendo pressionado.

Contabilizar o tempo que o botão vermelho se mantém pressionado.
Manter o LED piscando com o tempo que o botão foi pressionado.
O LED deve apagar sempre que o botão estiver pressionado.
*/

#include <stdio.h>
#include <string.h> 
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/irq.h"

const int LED_PIN = 5;
const int BTN_PIN = 28;

volatile int led_counter = 0;
volatile bool pressed_btn = false; 

volatile uint32_t start_ms = 0;
volatile bool new_time = false;

bool timer_callback(repeating_timer_t *rt){ // lê o estado atual do led e inverte
    bool estado_atual = gpio_get(LED_PIN);
    gpio_put(LED_PIN, !estado_atual);
    return true;
}

void btn_callback(uint gpio, uint32_t events){
    led_counter = 0;
    if (events == GPIO_IRQ_EDGE_FALL && gpio == BTN_PIN){
        start_ms = to_ms_since_boot(get_absolute_time());
        pressed_btn = true;
    } 

    if (events == GPIO_IRQ_EDGE_RISE && gpio == BTN_PIN){
        uint32_t finish_ms = to_ms_since_boot(get_absolute_time());
        led_counter = finish_ms - start_ms;  // conta para saber quantos ms o led deve piscar
        pressed_btn = false;
        new_time = true;
    }

}

void setup(){
    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN);
    gpio_set_irq_enabled_with_callback(BTN_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback);

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0); // Garante que começa apagado
}

int main() {
    stdio_init_all();

    setup();

    // estruturas para controlar o timer
    repeating_timer_t timer;
    bool active_timer = false;

    while (true) {

        if (pressed_btn){
            if(active_timer){
                cancel_repeating_timer(&timer); // cancela o timer 
                active_timer = false;
            }
            gpio_put(LED_PIN, 0); // apaga o led se o botão estiver apertado 
            
        } else if (new_time){
            new_time = false;
            if (led_counter>0){
                ass_repeating_timer_ms(led_counter, timer_callback, NULL, &timer);
                active_timer = true;
            }
        }

    }

    return 0;
}