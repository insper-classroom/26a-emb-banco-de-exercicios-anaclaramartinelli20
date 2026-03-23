#include <stdio.h>
#include <stdbool.h> // Faltava essa biblioteca para o "bool"
#include <stdint.h>  // Para o uint32_t
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h" // Para as funções de tempo

const int BTN_VERDE = 18;
const int BTN_AMARELO = 2;
const int BTN_VERMELHO = 3;

const int LED_VERDE = 15;
const int LED_AMARELO = 17;
const int LED_VERMELHO = 16;

// vetores de mapeamento
const int seq_leds[10] = {LED_AMARELO, LED_VERDE, LED_VERMELHO, LED_AMARELO, LED_VERDE, LED_AMARELO, LED_VERMELHO, LED_AMARELO, LED_VERDE, LED_AMARELO}; 
const int seq_botoes[10] = {BTN_AMARELO, BTN_VERDE, BTN_VERMELHO, BTN_AMARELO, BTN_VERDE, BTN_AMARELO, BTN_VERMELHO, BTN_AMARELO, BTN_VERDE, BTN_AMARELO};

volatile int rodada_atual = 1;  
volatile int passo_atual = 0; 
volatile int pontuacao = 0;

typedef enum{
    START,
    SHOW,
    PLAY
} state_t;

volatile state_t estado_atual = START;

volatile int btn_pressionado = -1;
volatile uint32_t ultimo_irq = 0;

void led_init (){ 
    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    
    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);

    gpio_init(LED_AMARELO);
    gpio_set_dir(LED_AMARELO, GPIO_OUT);
}

void btn_init (){ 
    gpio_init(BTN_VERDE);
    gpio_set_dir(BTN_VERDE, GPIO_IN);
    gpio_pull_up(BTN_VERDE);
    
    gpio_init(BTN_AMARELO);
    gpio_set_dir(BTN_AMARELO, GPIO_IN);
    gpio_pull_up(BTN_AMARELO);
    
    gpio_init(BTN_VERMELHO);
    gpio_set_dir(BTN_VERMELHO, GPIO_IN);
    gpio_pull_up(BTN_VERMELHO);
}

void btn_callback(uint gpio, uint32_t events){
    // Debounce para evitar cliques duplos fantasma
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());
    if (tempo_atual - ultimo_irq < 250) return;
    ultimo_irq = tempo_atual;

    if (estado_atual == START){
        if(gpio == BTN_VERDE){
            estado_atual = SHOW;
        }
    } else if(estado_atual == PLAY){
        btn_pressionado = gpio; 
    }
}

int main() {
    stdio_init_all();

    led_init();
    btn_init();

    gpio_set_irq_enabled_with_callback(BTN_VERDE, GPIO_IRQ_EDGE_FALL, true, &btn_callback);
    gpio_set_irq_enabled(BTN_AMARELO, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(BTN_VERMELHO, GPIO_IRQ_EDGE_FALL, true);

    while (true) {
        switch (estado_atual)
        {
        case START:
            rodada_atual = 1;
            passo_atual = 0;
            pontuacao = 0;
            btn_pressionado = -1;
            break;
        
        case SHOW:
            sleep_ms(300);

            for (int i=0; i<rodada_atual; i++){
                gpio_put(seq_leds[i], 1); 
                sleep_ms(300);

                gpio_put(seq_leds[i], 0);  
                sleep_ms(300);
            }

            btn_pressionado = -1; // Limpa se o jogador apertou durante o show
            estado_atual = PLAY;
            break;

        case PLAY:
            if(btn_pressionado != -1){
                int pino_esperado = seq_botoes[passo_atual];

                if (btn_pressionado == pino_esperado){
                    passo_atual++;
                    btn_pressionado = -1; 

                    if (passo_atual == rodada_atual){
                        if (rodada_atual == 10){ 
                            pontuacao = rodada_atual;
                            printf("Points %d\n", pontuacao);
                            estado_atual = START;
                        } else {
                            rodada_atual++;
                            passo_atual = 0; // Zera o passo para a próxima rodada
                            estado_atual = SHOW;
                        }
                    } 
                } else { 
                    pontuacao = rodada_atual - 1; // Ajusta a pontuação de onde ele perdeu
                    printf("Points %d\n",  pontuacao);
                    estado_atual = START;
                }
            }
            break;
        }
    }
}