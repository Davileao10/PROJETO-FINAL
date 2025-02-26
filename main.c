/**
 * Alarme com RP2040 usando display SSD1306, botões, LEDs WS2812 e buzzers para controle
 * BitdogLab
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include "pico/stdlib.h"
 #include "hardware/pwm.h"
 #include "hardware/irq.h"
 #include "hardware/gpio.h"
 #include "hardware/pio.h"
 #include "ws2812.pio.h"
 #include "inc/ssd1306.h"
 #include "inc/pwm_irq.h"
 
 #define BUZZER_PIN_1 10   // Primeiro buzzer
 #define BUZZER_PIN_2 21   // Segundo buzzer
 #define BUTTON_A_PIN 5    // Botão A (incrementa)
 #define BUTTON_B_PIN 6    // Botão B (decrementa)
 #define BUTTON_START_PIN 22 // Botão do joystick (inicia)
 #define SDA_PIN 14        // I2C SDA
 #define SCL_PIN 15        // I2C SCL
 #define WS2812_PIN 7      // Matriz 5x5 RGB (WS2812)
 #define NUM_PIXELS 25     // 5x5 LEDs
 #define IS_RGBW false     // Não é RGBW, apenas RGB
 
 #define DEBOUNCE_TIME 200 // Tempo de debounce em ms
 #define INCREMENT_VALUE 30 // Incremento/decremento em segundos
 
 volatile uint32_t alarm_time = 60;
 volatile bool alarm_active = false;
 volatile uint32_t current_time = 0;
 volatile uint64_t last_button_press = 0;
 
 // Buffer para números na matriz (0 a 9) com 5x5 pixels
 bool numeros[10][NUM_PIXELS] = {
     {0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0}, // 0
     {0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0}, // 1
     {0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0}, // 2
     {0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0}, // 3
     {0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0}, // 4
     {0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0}, // 5
     {0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0}, // 6
     {0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0}, // 7
     {0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0}, // 8
     {0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 0}  // 9
 };
 
 static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
     return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
 }
 
 static inline void put_pixel(uint32_t pixel_grb, PIO pio, uint sm, uint pin) {
     pio_sm_put_blocking(pio, sm, pixel_grb << 8u);
 }
 
 void display_number(int number, PIO pio, uint sm, uint pin) {
     uint32_t color;
     switch (number) {
         case 5: color = urgb_u32(75, 0, 0); break;   // Vermelho
         case 4: color = urgb_u32(0, 75, 0); break;   // Verde
         case 3: color = urgb_u32(0, 0, 75); break;   // Azul
         case 2: color = urgb_u32(75, 75, 0); break; // Amarelo
         case 1: color = urgb_u32(55, 55, 55); break; // Branco
         default: color = urgb_u32(55, 0, 0); break;  // Padrão vermelho
     }
     for (int i = 0; i < NUM_PIXELS; i++) {
         put_pixel(numeros[number][i] ? color : 0, pio, sm, pin);
     }
 }
 
 void display_all_red(PIO pio, uint sm, uint pin) {
     uint32_t color = urgb_u32(55, 0, 0); // Todos os pixels em vermelho
     for (int i = 0; i < NUM_PIXELS; i++) {
         put_pixel(color, pio, sm, pin);
     }
 }
 
 void button_callback(uint gpio, uint32_t events) {
     uint64_t current_time_ms = to_ms_since_boot(get_absolute_time());
     if (current_time_ms - last_button_press < DEBOUNCE_TIME) return;
     last_button_press = current_time_ms;
 
     if (!alarm_active) {
         if (gpio == BUTTON_A_PIN) {
             alarm_time += INCREMENT_VALUE;
             printf("Tempo do alarme: %lu segundos\n", alarm_time);
         } else if (gpio == BUTTON_B_PIN && alarm_time >= INCREMENT_VALUE) {
             alarm_time -= INCREMENT_VALUE;
             printf("Tempo do alarme: %lu segundos\n", alarm_time);
         }
     }
 }
 
 void update_display(ssd1306_t *disp) {
     char buffer[30];
     ssd1306_clear(disp);
     
     if (alarm_active) {
         uint32_t minutes = current_time / 60;
         uint32_t seconds = current_time % 60;
         ssd1306_draw_string(disp, 0, 0, 1, "ALARME ATIVO");
         sprintf(buffer, "Tempo: %02lu:%02lu", minutes, seconds);
         ssd1306_draw_string(disp, 0, 16, 1, buffer);
         if (current_time <= 5) {
             ssd1306_draw_string(disp, 0, 32, 1, "FINALIZANDO!");
         }
     } else {
         uint32_t minutes = alarm_time / 60;
         uint32_t seconds = alarm_time % 60;
         ssd1306_draw_string(disp, 0, 0, 1, "CONFIGURAR ALARME");
         sprintf(buffer, "Tempo: %02lu:%02lu", minutes, seconds);
         ssd1306_draw_string(disp, 0, 16, 1, buffer);
         ssd1306_draw_string(disp, 0, 32, 1, "A+ 30s | B- 30s");
         ssd1306_draw_string(disp, 0, 48, 1, "START: Joystick");
     }
     ssd1306_show(disp);
 }
 
 void update_leds(uint32_t time_left, PIO pio, uint sm, uint pin) {
     if (time_left == 5 || time_left == 4 || time_left == 3 || time_left == 2 || time_left == 1) {
         switch (time_left) {
             case 5: buzzer_set_frequency(2700); break;
             case 4: buzzer_set_frequency(2500); break;
             case 3: buzzer_set_frequency(2300); break;
             case 2: buzzer_set_frequency(2100); break;
             case 1: buzzer_set_frequency(1900); break;
         }
         display_number(time_left, pio, sm, pin);  // Mostra o número com cor específica
         sleep_ms(750);  // Pisca por 750ms
         for (int i = 0; i < NUM_PIXELS; i++) {
             put_pixel(0, pio, sm, pin);  // Apaga
         }
         sleep_ms(250);  // Desliga por 250ms
         buzzer_set_frequency(2700);  // Volta à frequência padrão após o pisca
     } else if (time_left == 0) {
         display_all_red(pio, sm, pin);  // Acende todos em vermelho
         buzzer_set_frequency(2700);  // Volta à frequência padrão
     }
 }
 
 void start_alarm() {
     if (!alarm_active && alarm_time > 0) {
         alarm_active = true;
         current_time = alarm_time;
         printf("Alarme iniciado! Tempo: %lu segundos\n", alarm_time);
     }
 }
 
 void stop_alarm() {
     alarm_active = false;
     buzzer_off();
     PIO pio = pio0;
     uint sm = 0;
     for (int i = 0; i < NUM_PIXELS; i++) {
         put_pixel(0, pio, sm, WS2812_PIN);  // Apaga a matriz
     }
     printf("Alarme parado!\n");
 }
 
 int main() {
     stdio_init_all();
     
     i2c_init(i2c1, 100000);
     gpio_set_function(14, GPIO_FUNC_I2C);
     gpio_set_function(15, GPIO_FUNC_I2C);
     gpio_pull_up(14);
     gpio_pull_up(15);
 
     ssd1306_t disp;
     disp.external_vcc = false;
     ssd1306_init(&disp, 128, 64, 0x3C, i2c1);
     ssd1306_clear(&disp);
     
     pwm_irq_setup(BUZZER_PIN_1);
     pwm_irq_setup(BUZZER_PIN_2);
     
     gpio_init(BUTTON_A_PIN);
     gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
     gpio_pull_up(BUTTON_A_PIN);
     
     gpio_init(BUTTON_B_PIN);
     gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
     gpio_pull_up(BUTTON_B_PIN);
     
     gpio_init(BUTTON_START_PIN);
     gpio_set_dir(BUTTON_START_PIN, GPIO_IN);
     gpio_pull_up(BUTTON_START_PIN);
     
     // Inicializar PIO para WS2812
     PIO pio = pio0;
     uint sm = 0;
     uint offset = pio_add_program(pio, &ws2812_program);
     ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);
     
     gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, 4, true, &button_callback);
     gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, 4, true, &button_callback);
     
     update_display(&disp);
     
     while (true) {
         if (!alarm_active) {
             if (!gpio_get(BUTTON_START_PIN)) {
                 start_alarm();
                 sleep_ms(500);
             }
         }
         
         update_display(&disp);
         
         if (alarm_active) {
             if (current_time > 0) {
                 sleep_ms(1000);  // Garantir sincronia com o relógio
                 current_time--;
                 if (current_time <= 5) {
                     buzzer_on();
                     sleep_ms(100);
                     buzzer_off();
                 }
                 if (current_time == 5 || current_time == 4 || current_time == 3 || current_time == 2 || current_time == 1) {
                     update_leds(current_time, pio, sm, WS2812_PIN);
                 }
             } else {
                 printf("TEMPO ESGOTADO! ALARME!\n");
                 update_leds(0, pio, sm, WS2812_PIN);  // Acende todos em vermelho
                 buzzer_on();
                 sleep_ms(5000);
                 buzzer_off();
                 stop_alarm();
             }
         } else {
             sleep_ms(100);
         }
     }
     
     return 0;
 }