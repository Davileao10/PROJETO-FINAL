/**
 * Copyright (c) 2021 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

 #ifndef _SSD1306_H
 #define _SSD1306_H
 
 #include "hardware/i2c.h"
 
 // Estrutura para controlar o display SSD1306
 typedef struct {
     uint8_t width;       // Largura do display em pixels
     uint8_t height;      // Altura do display em pixels
     uint8_t pages;       // Pages = height / 8
     uint8_t address;     // Endereço I2C do dispositivo
     bool external_vcc;   // Se usa fonte de alimentação externa
     uint8_t *buffer;     // Buffer de dados para o display
     size_t bufsize;      // Tamanho do buffer
     i2c_inst_t *i2c_i;   // Instância I2C
 } ssd1306_t;
 
 // Inicializa o display SSD1306
 void ssd1306_init(ssd1306_t *disp, uint8_t width, uint8_t height, uint8_t address, i2c_inst_t *i2c_instance);
 
 // Libera a memória usada pelo display
 void ssd1306_deinit(ssd1306_t *disp);
 
 // Limpa o buffer do display
 void ssd1306_clear(ssd1306_t *disp);
 
 // Envia os dados do buffer para o display
 void ssd1306_show(ssd1306_t *disp);
 
 // Desenha um pixel no buffer
 void ssd1306_draw_pixel(ssd1306_t *disp, uint8_t x, uint8_t y);
 
 // Apaga um pixel no buffer
 void ssd1306_clear_pixel(ssd1306_t *disp, uint8_t x, uint8_t y);
 
 // Desenha caractere no buffer
 void ssd1306_draw_char(ssd1306_t *disp, uint8_t x, uint8_t y, uint8_t scale, char c);
 
 // Desenha string no buffer
 void ssd1306_draw_string(ssd1306_t *disp, uint8_t x, uint8_t y, uint8_t scale, const char *s);
 
 // Configura brilho do display (0-255)
 void ssd1306_set_contrast(ssd1306_t *disp, uint8_t contrast);
 
 // Inverte as cores do display
 void ssd1306_invert(ssd1306_t *disp, bool invert);
 
 #endif // _SSD1306_H