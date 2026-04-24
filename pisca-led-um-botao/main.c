#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

/*
- Os leds devem piscar somente quando o botão for pressionado. -> borda de descida
- Eles devem piscar por 5 segundos, depois devem parar de piscar automaticamente. 

O período que cada LED deve piscar é:

- Led amarelo: 500ms
- Led azul: 150ms

**Regras de implementação do firmware:**

- Baremetal (sem RTOS).
- utilizar exatamente **`3`** timers independentes:
    - Um para o LED amarelo;
    - Um para o LED Azul;
    - Um para o alarme que irá contar 5 segundos.
- Os leds devem parar de piscar apagados.
- Deve trabalhar com interrupções nos botões.  
- Não é permitido usar `sleep_ms(), sleep_us(), get_absolute_time()`.
- **printf** pode atrapalhar o tempo de simulação, comenta/remova antes de testar.

*/

const int BTN_VERDE = 18;

const int LED_VERDE = 15;
const int LED_VERMELHO = 14;
const int LED_AMARELO = 13;

volatile int led_y_flag = 0;
volatile int led_g_flag = 0;

// variável para o timer
volatile int g_timer = 0;
volatile int y_timer = 0;
volatile int r_timer = 0;

repeating_timer_t timer_amarelo;
repeating_timer_t timer_azul;
alarm_id_t alarme_5s;

volatile bool rodando = false;
volatile bool btn_pressionado = false;


// Callback do LED Azul 
bool timer_y_callback(repeating_timer_t *rt) {
    gpio_put(LED_AMARELO, !gpio_get(LED_AMARELO));
    return true; 
}

// Callback do LED Azul 
bool timer_b_callback(repeating_timer_t *rt) {
    gpio_put(LED_VERMELHO, !gpio_get(LED_VERMELHO));
    return true; 
}

// Callback do Alarme de 5 Segundos
int64_t alarme_5s_callback(alarm_id_t id, void *user_data) {
    // Cancela os timers que estão fazendo os LEDs piscarem
    cancel_repeating_timer(&timer_amarelo);
    cancel_repeating_timer(&timer_azul);

    gpio_put(LED_AMARELO, 0);
    gpio_put(LED_VERMELHO, 0);

    rodando = false; 

    return 0; 
}

void btn_callback(uint gpio, uint32_t events) {
    if(!rodando){
        btn_pressionado = true;
    }
}

void setup(){    
    gpio_init(BTN_VERDE);
    gpio_set_dir(BTN_VERDE, GPIO_IN);

    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    
    gpio_init(LED_AMARELO);
    gpio_set_dir(LED_AMARELO, GPIO_OUT);
}

int main() {
    stdio_init_all();
    setup();
    gpio_set_irq_enabled_with_callback(BTN_VERDE, GPIO_IRQ_EDGE_FALL, true, &btn_callback);

    while (true) {

        // if(btn_pressionado){
        //     btn_pressionado = false;
        //     rodando = true;

        //     add_repeating_timer_ms(500, timer_y_callback, NULL, &timer_amarelo);
        //     add_repeating_timer_ms(150, timer_b_callback, NULL, &timer_azul);

        //     add_alarm_in_ms(5000, alarme_5s_callback, NULL, false);

        // }

    }

    return 0;
}
