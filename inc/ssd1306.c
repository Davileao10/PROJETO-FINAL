/**
 * Copyright (c) 2021 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

 #include <stdio.h>
 #include <string.h>
 #include <stdlib.h>
 #include "pico/stdlib.h"
 #include "hardware/i2c.h"
 #include "ssd1306.h"
 #include "font.h"
 
 // Comandos SSD1306
 #define SSD1306_SET_CONTRAST            0x81
 #define SSD1306_SET_ENTIRE_ON           0xA4
 #define SSD1306_SET_NORM_INV            0xA6
 #define SSD1306_SET_DISP                0xAE
 #define SSD1306_SET_MEM_ADDR            0x20
 #define SSD1306_SET_COL_ADDR            0x21
 #define SSD1306_SET_PAGE_ADDR           0x22
 #define SSD1306_SET_DISP_START_LINE     0x40
 #define SSD1306_SET_SEG_REMAP           0xA0
 #define SSD1306_SET_MUX_RATIO           0xA8
 #define SSD1306_SET_COM_OUT_DIR         0xC0
 #define SSD1306_SET_DISP_OFFSET         0xD3
 #define SSD1306_SET_COM_PIN_CFG         0xDA
 #define SSD1306_SET_DISP_CLK_DIV        0xD5
 #define SSD1306_SET_PRECHARGE           0xD9
 #define SSD1306_SET_VCOM_DESEL          0xDB
 #define SSD1306_SET_CHARGE_PUMP         0x8D
 
 // Função para enviar comando ao SSD1306
 static void send_command(ssd1306_t *disp, uint8_t command) {
     uint8_t buffer[2] = {0x00, command}; // Co = 0, D/C = 0
     i2c_write_blocking(disp->i2c_i, disp->address, buffer, 2, false);
 }
 
 // Função para inicializar o display SSD1306
 void ssd1306_init(ssd1306_t *disp, uint8_t width, uint8_t height, uint8_t address, i2c_inst_t *i2c_instance) {
     disp->width = width;
     disp->height = height;
     disp->pages = height / 8;
     disp->address = address;
     disp->i2c_i = i2c_instance;
     
     // Alocar buffer para o display (cada byte = 8 pixels verticais)
     disp->bufsize = (disp->width * disp->pages);
     disp->buffer = malloc(disp->bufsize);
     if (!disp->buffer) {
         printf("Erro: Não foi possível alocar buffer para o display\n");
         return;
     }
     
     // Limpar o buffer
     memset(disp->buffer, 0, disp->bufsize);
     
     // Sequência de inicialização do display
     send_command(disp, SSD1306_SET_DISP | 0x00);  // display off
     
     send_command(disp, SSD1306_SET_MEM_ADDR);     // set memory address mode
     send_command(disp, 0x00);                     // horizontal addressing mode
     
     send_command(disp, SSD1306_SET_DISP_START_LINE | 0x00); // start at line 0
     send_command(disp, SSD1306_SET_SEG_REMAP | 0x01);       // column addr 127 mapped to SEG0
     
     send_command(disp, SSD1306_SET_MUX_RATIO);    // set multiplex ratio
     send_command(disp, disp->height - 1);         // height-1
     
     send_command(disp, SSD1306_SET_COM_OUT_DIR | 0x08); // scan from COM[N-1] to COM0
     send_command(disp, SSD1306_SET_DISP_OFFSET);        // set display offset
     send_command(disp, 0x00);                           // no offset
     
     send_command(disp, SSD1306_SET_COM_PIN_CFG);  // set COM pins hardware config
     if (disp->width > 2 * disp->height) {
         send_command(disp, 0x02);                 // sequential COM pin config, disable remap
     } else {
         send_command(disp, 0x12);                 // alt COM pin config, enable remap
     }
     
     send_command(disp, SSD1306_SET_DISP_CLK_DIV); // set display clock div
     send_command(disp, 0x80);                     // suggested ratio
     
     send_command(disp, SSD1306_SET_PRECHARGE);    // set precharge period
     send_command(disp, disp->external_vcc ? 0x22 : 0xF1);
     
     send_command(disp, SSD1306_SET_VCOM_DESEL);   // set vcomh deselect level
     send_command(disp, 0x30);                     // 0.83 * Vcc
     
     send_command(disp, SSD1306_SET_CONTRAST);     // set contrast control
     send_command(disp, 0xFF);
     
     send_command(disp, SSD1306_SET_ENTIRE_ON);    // output follows RAM contents
     send_command(disp, SSD1306_SET_NORM_INV);     // normal display (não invertido)
     
     send_command(disp, SSD1306_SET_CHARGE_PUMP);  // set charge pump
     send_command(disp, disp->external_vcc ? 0x10 : 0x14);
     
     send_command(disp, SSD1306_SET_DISP | 0x01);  // display on
 }
 
 // Libera a memória do display
 void ssd1306_deinit(ssd1306_t *disp) {
     if (disp->buffer) {
         free(disp->buffer);
         disp->buffer = NULL;
     }
 }
 
 // Limpa o buffer do display
 void ssd1306_clear(ssd1306_t *disp) {
     memset(disp->buffer, 0, disp->bufsize);
 }
 
 // Envia o buffer para o display
 void ssd1306_show(ssd1306_t *disp) {
     // Define área de endereço da coluna
     send_command(disp, SSD1306_SET_COL_ADDR);
     send_command(disp, 0);
     send_command(disp, disp->width - 1);
     
     // Define área de endereço da página
     send_command(disp, SSD1306_SET_PAGE_ADDR);
     send_command(disp, 0);
     send_command(disp, disp->pages - 1);
     
     // Copia o buffer para o display
     uint8_t *buf = malloc(disp->bufsize + 1);
     if (buf) {
         buf[0] = 0x40; // Co = 0, D/C = 1
         memcpy(buf + 1, disp->buffer, disp->bufsize);
         i2c_write_blocking(disp->i2c_i, disp->address, buf, disp->bufsize + 1, false);
         free(buf);
     }
 }
 
 // Desenha um pixel
 void ssd1306_draw_pixel(ssd1306_t *disp, uint8_t x, uint8_t y) {
     if (x >= disp->width || y >= disp->height) return;
     
     // Converte coordenadas x,y para posição no buffer
     uint8_t page = y / 8;
     uint8_t bit = y % 8;
     uint16_t idx = x + page * disp->width;
     
     // Define o bit apropriado
     disp->buffer[idx] |= (1 << bit);
 }
 
 // Limpa um pixel
 void ssd1306_clear_pixel(ssd1306_t *disp, uint8_t x, uint8_t y) {
     if (x >= disp->width || y >= disp->height) return;
     
     // Converte coordenadas x,y para posição no buffer
     uint8_t page = y / 8;
     uint8_t bit = y % 8;
     uint16_t idx = x + page * disp->width;
     
     // Limpa o bit apropriado
     disp->buffer[idx] &= ~(1 << bit);
 }
 
 // Desenha um caractere
 void ssd1306_draw_char(ssd1306_t *disp, uint8_t x, uint8_t y, uint8_t scale, char c) {
     if (c < 32 || c > 127) c = '?';
     
     // Cada char tem 5x7 pixels, espaçado por 1 pixel horizontalmente
     for (uint8_t i = 0; i < 5; i++) {
         uint8_t line = font[(c - 32) * 5 + i];
         for (uint8_t j = 0; j < 7; j++) {
             if (line & (1 << j)) {
                 if (scale == 1) {
                     ssd1306_draw_pixel(disp, x + i, y + j);
                 } else {
                     for (uint8_t sx = 0; sx < scale; sx++) {
                         for (uint8_t sy = 0; sy < scale; sy++) {
                             ssd1306_draw_pixel(disp, x + i * scale + sx, y + j * scale + sy);
                         }
                     }
                 }
             }
         }
     }
 }
 
 // Desenha uma string
 void ssd1306_draw_string(ssd1306_t *disp, uint8_t x, uint8_t y, uint8_t scale, const char *s) {
     while (*s) {
         ssd1306_draw_char(disp, x, y, scale, *s);
         x += 6 * scale;  // 6 = 5 pixels de largura + 1 pixel de espaço
         if (x + 6 * scale > disp->width) {
             x = 0;
             y += 8 * scale;  // 8 = 7 pixels de altura + 1 pixel de espaço
             if (y + 8 * scale > disp->height) {
                 break;
             }
         }
         s++;
     }
 }
 
 // Configura o contraste do display
 void ssd1306_set_contrast(ssd1306_t *disp, uint8_t contrast) {
     send_command(disp, SSD1306_SET_CONTRAST);
     send_command(disp, contrast);
 }
 
 // Inverte as cores do display
 void ssd1306_invert(ssd1306_t *disp, bool invert) {
     send_command(disp, SSD1306_SET_NORM_INV | (invert ? 1 : 0));
 }