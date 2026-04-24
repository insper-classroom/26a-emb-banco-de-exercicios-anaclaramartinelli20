#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/gpio.h"

const int BTN_Y = 28;
const int BTN_B = 22;
const int BTN_G = 18;

const int LED_Y = 6;
const int LED_B = 10;
const int LED_G = 14;

volatile int btn_y_flag = 0; 
volatile int btn_b_flag = 0; 
volatile int btn_g_flag = 0; 

// Variável para armazenar o tempo da última atividade (PASSO 1)
volatile uint32_t tempo_ultimo_clique = 0;

void btn_callback(uint gpio, uint32_t events) {
    // Atualiza o tempo sempre que qualquer botão for mexido (PASSO 2)
    tempo_ultimo_clique = to_ms_since_boot(get_absolute_time());

    if (events == 0x4) {  // Borda de descida 
      if (gpio == BTN_Y){
        btn_y_flag = !btn_y_flag; 
      } 
      if (gpio == BTN_G){
        btn_g_flag = 1; 
      }
    }

    if (events == 0x8) {  // Borda de subida 
      if (gpio == BTN_B){
        btn_b_flag = !btn_b_flag; 
      } 
      if (gpio == BTN_G){
        btn_g_flag = 0; 
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
  gpio_put(LED_Y, 0); 

  gpio_init(BTN_B);
  gpio_set_dir(BTN_B, GPIO_IN);
  gpio_pull_up(BTN_B);
  gpio_set_irq_enabled_with_callback(BTN_B, GPIO_IRQ_EDGE_RISE, true, &btn_callback);

  gpio_init(LED_B);
  gpio_set_dir(LED_B, GPIO_OUT);
  gpio_put(LED_B, 0); 

  gpio_init(BTN_G);
  gpio_set_dir(BTN_G, GPIO_IN);
  gpio_pull_up(BTN_G);
  gpio_set_irq_enabled_with_callback(BTN_G, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback); 

  gpio_init(LED_G);
  gpio_set_dir(LED_G, GPIO_OUT);
  gpio_put(LED_G, 0); 
}

int main() {
  stdio_init_all();
  setup();
 
  bool estado_pisca = false; 

  // Inicializa o contador para o timeout não disparar logo ao ligar a placa
  tempo_ultimo_clique = to_ms_since_boot(get_absolute_time());

  while (true) {
    
    // --- VERIFICAÇÃO DE TIMEOUT (PASSO 3) ---
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());
    if ((tempo_atual - tempo_ultimo_clique) >= 5000) {
        
        // Timeout atingido! Reseta o estado de todos os LEDs
        btn_y_flag = 0;
        btn_b_flag = 0;
        btn_g_flag = 0;
        
        // Atualiza o tempo para não ficar disparando o timeout em loop
        tempo_ultimo_clique = tempo_atual; 
    }
    // ----------------------------------------

    if(btn_y_flag == 1){
      gpio_put(LED_Y, estado_pisca);
    } else{
      gpio_put(LED_Y, 0);
    }

    if(btn_b_flag == 1){
      gpio_put(LED_B, estado_pisca);
    } else{
      gpio_put(LED_B, 0);
    }

    if(btn_g_flag == 1){
      gpio_put(LED_G, estado_pisca);
    } else{
      gpio_put(LED_G, 0);
    }

    estado_pisca = !estado_pisca;
    sleep_ms(200);
  }
}