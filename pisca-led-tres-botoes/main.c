#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/gpio.h"

/*
- `AMARELO`: Ativar / Desativar ao **apertar** o botão  -> borda de descida ativa ou desativa
- `AZUL`: Ativar / Desativar ao **soltar** o botão  -> borda de subida ativa ou desativa
- `VERDE`: Ativado enquanto **apertado** e desativado quando **solto**.  borda de descida ativa e borda de subida desativa

- Ativar = LED PISCANDO a cada 200ms
- Desativar = LED APAGADO
- **Os LEDs devem piscar juntos na mesma frequência e sincronizados!**

- Usar delay de 200ms, caso contrário pode falhar no wokwi
- Não pode usar nenhum timer
*/

const int BTN_Y = 28;
const int BTN_B = 22;
const int BTN_G = 18;

const int LED_Y = 6;
const int LED_B = 10;
const int LED_G = 14;

volatile int btn_y_flag = 0; // muda de estado se houve borda de descida
volatile int btn_b_flag = 0; // muda de estado se houve borda de subida
volatile int btn_g_flag = 0; // borda de descida ativa e borda de subida desativa

volatile bool led_y_ligado = false;
volatile bool led_b_ligado = false;
volatile bool led_g_ligado = false;

// Callback da interrupção dos botões
void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) {  // Borda de descida 
      if (gpio==BTN_Y){
        btn_y_flag = !btn_y_flag; // inverte o estado
      } 

      if (gpio==BTN_G){
        btn_g_flag = 1; // ativa o verde
      }
    }

    if (events == 0x8) {  // Borda de subida 
      if (gpio==BTN_B){
        btn_b_flag = !btn_b_flag; // inverte o estado
      } 

      if (gpio==BTN_G){
        btn_g_flag = 0; // desativa o verde
        gpio_put(LED_G, 0);
      }
    }
}

void setup(){
  gpio_init(BTN_Y);
  gpio_set_dir(BTN_Y, GPIO_IN);
  gpio_pull_up(BTN_Y);
  gpio_set_irq_enabled_with_callback(BTN_Y, GPIO_IRQ_EDGE_FALL, true, &btn_callback);

  gpio_init(LED_Y);
  gpio_set_dir(LED_Y, GPIO_OUT);
  gpio_put(LED_Y, 0); // Garante que começa apagado

  gpio_init(BTN_B);
  gpio_set_dir(BTN_B, GPIO_IN);
  gpio_pull_up(BTN_B);
  gpio_set_irq_enabled_with_callback(BTN_B, GPIO_IRQ_EDGE_RISE, true, &btn_callback);

  gpio_init(LED_B);
  gpio_set_dir(LED_B, GPIO_OUT);
  gpio_put(LED_B, 0); // Garante que começa apagado

  gpio_init(BTN_G);
  gpio_set_dir(BTN_G, GPIO_IN);
  gpio_pull_up(BTN_G);
  gpio_set_irq_enabled_with_callback(BTN_G, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback); // ativando as duas bordas para o botão

  gpio_init(LED_G);
  gpio_set_dir(LED_G, GPIO_OUT);
  gpio_put(LED_G, 0); // Garante que começa apagado
}

int main() {
  stdio_init_all();

  setup();
 
  bool estado_pisca = false; // flag para inverter o estado dos leds de maneira sincronizada

  while (true) {

    if(btn_y_flag==1){
      gpio_put(LED_Y, estado_pisca);
    } else{
      gpio_put(LED_Y, 0);
    }

    if(btn_b_flag==1){
      gpio_put(LED_B, estado_pisca);
    } else{
      gpio_put(LED_B, 0);
    }

    if(btn_g_flag==1){
      gpio_put(LED_G, estado_pisca);
    } else{
      gpio_put(LED_G, 0);
    }

    estado_pisca = !estado_pisca;

    sleep_ms(200);
  }
}