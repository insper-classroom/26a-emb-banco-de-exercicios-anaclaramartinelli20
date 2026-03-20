#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/gpio.h"

const int BTN_PIN = 22;
const int SW_PIN = 28;

const int LED1 = 2;
const int LED2 = 3;
const int LED3 = 4;
const int LED4 = 5;
const int LED5 = 6;

volatile int counter = 0; // valor atual da barra de leds
volatile int sw_state = 1; // estado da chave começa em 1 devido ao pull up

int bar_init(){
    gpio_init(LED1);
    gpio_set_dir(LED1, GPIO_OUT);

    gpio_init(LED2);
    gpio_set_dir(LED2, GPIO_OUT);

    gpio_init(LED3);
    gpio_set_dir(LED3, GPIO_OUT);
    
    gpio_init(LED4);
    gpio_set_dir(LED4, GPIO_OUT);
    
    gpio_init(LED5);
    gpio_set_dir(LED5, GPIO_OUT);
}

int bar_display(int val){ 
    gpio_put(LED1, val>=1); // se val>=1 o led acende
    gpio_put(LED2, val>=2);
    gpio_put(LED3, val>=3);
    gpio_put(LED4, val>=4);
    gpio_put(LED5, val>=5);
}

void callback(unit gpio, unit32_t events){
    /* 
    SEPARAR QUEM CHAMOU A INTERRUPÇÃO:
        1) Chave: atualizar a variável sw_state
        2) Botão: olhar para a variável sw_state e atualizar o counter + leds
    */ 

    // Se foi a chave (caso 1)
    if (gpio == SW_PIN){
        if (events & GPIO_IRQ_EDGE_FALL){ // se houve borda de descida
            sw_state = 0; // chave é ligada no GND
        } else if (events & GPIO_IRQ_EDGE_RISE){
            sw_state = 1; // chave é desligada do GND
        }
    }

    // Se foi o botão
    if (gpio == BTN_PIN){
        if (events & GPIO_IRQ_EDGE_FALL){ // se houve borda de descida
            
           if (sw_state == 0){
                counter++;
                if (counter > 5){
                    counter = 5; // não pode passar de 5
                }
           } else {
                counter--;
                if (counter < 0){
                    counter = 0; // não pode ser negativo
                }
           }

           bar_display(counter);
        }
    }
}

int main() {
    stdio_init_all();

    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN);

    gpio_init(SW_PIN);
    gpio_set_dir(SW_PIN, GPIO_IN);
    gpio_pull_up(SW_PIN);

    bar_init(); // inicializa os leds

    // configuração das interrupções
    gpio_set_irq_enabled_with_callback(BTN_PIN, GPIO_IRQ_EDGE_FALL, true, &callback);
    gpio_set_irq_enabled_with_callback(SW_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE , true, &gpio_callback);


    while (true) {
        tight_loop_contents(); // continua executando o programa
    }
}
