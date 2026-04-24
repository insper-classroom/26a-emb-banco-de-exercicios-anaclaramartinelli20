#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/gpio.h"
#include "hardware/timer.h" // Biblioteca de timers adicionada

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

const int BTN_AMARELO = 28;
const int BTN_AZUL = 22;
const int BTN_VERDE = 18;

const int LED_AMARELO = 6;
const int LED_AZUL = 10;
const int LED_VERDE = 14;

void setup(){
  gpio_init(BTN_AMARELO);
  gpio_set_dir(BTN_AMARELO, GPIO_IN);
  gpio_set_irq_enabled_with_callback(BTN_AMARELO, GPIO_IRQ_EDGE_FALL, true, &btn_callback);

  gpio_init(BTN_AZUL);
  gpio_set_dir(BTN_AZUL, GPIO_IN);
  gpio_set_irq_enabled_with_callback(BTN_AZUL, GPIO_IRQ_EDGE_RISE, true, &btn_callback);
  
  gpio_init(BTN_VERDE);
  gpio_set_dir(BTN_VERDE, GPIO_IN);
  gpio_set_irq_enabled_with_callback(BTN_VERDE, GPIO_IRQ_EDGE_FALL || GPIO_IRQ_EDGE_RISE, true, &btn_callback);

  gpio_init(LED_AMARELO);
  gpio_set_dir(LED_AMARELO, GPIO_OUT);

  gpio_init(LED_AZUL);
  gpio_set_dir(LED_AZUL, GPIO_OUT);

  gpio_init(LED_VERDE);
  gpio_set_dir(LED_VERDE, GPIO_OUT);
}

int main() {
  stdio_init_all();

  setup();

  while (true) {
  }
}