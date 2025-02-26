# Alarme Interativo com Contagem Regressiva na BitDogLab

## Descrição do Projeto
Este projeto apresenta um sistema embarcado inovador na forma de um alarme interativo, desenvolvido para a placa BitDogLab, utilizando um microcontrolador RP2040. O sistema combina um display OLED, botões, buzzers e uma matriz de LEDs RGB 5x5 para oferecer uma experiência visual e sonora dinâmica.

## Link
- **Vídeo de Demonstração:** ([https://drive.google.com/file/d/18f-KX2Qx_i7-tpfUvIMdOArqE_WXhsYn/view?usp=drivesdk]

## Objetivos
- Desenvolver um alarme funcional com contagem regressiva configurável.
- Fornecer feedback visual e sonoro através de LEDs RGB e buzzers.
- Interatividade via display SSD1306 e botões físicos.

## Funcionamento
- Configuração do tempo (incremento/decremento de 30 segundos) usando os botões GP5 (A) e GP6 (B).
- Início da contagem regressiva pelo botão joystick (GP22).
- Nos últimos 5 segundos, a matriz de LEDs exibe uma contagem colorida e o buzzer emite sons variados.
- Ao atingir 0 segundos, todos os LEDs acendem em vermelho e os buzzers soam por 5 segundos.

## Hardware Utilizado
- **Placa:** BitDogLab com microcontrolador RP2040
- **Display:** SSD1306 OLED (I2C)
- **Matriz de LEDs:** 5x5 RGB (WS2812)
- **Buzzers:** Dois buzzers controlados por PWM
- **Botões:** GP5, GP6 e GP22

## Pinagem
- **GP5:** Botão A (entrada, pull-up)
- **GP6:** Botão B (entrada, pull-up)
- **GP22:** Botão Joystick (entrada, pull-up)
- **GP14:** SDA (I2C para display)
- **GP15:** SCL (I2C para display)
- **GP10:** Buzzer-B (PWM)
- **GP21:** Buzzer-A (PWM)
- **GP7:** Matriz 5x5 RGB (WS2812)

## Software
- **Linguagem:** C
- **SDK:** Pico SDK 2.1.1
- **IDE:** Visual Studio Code
- **Compilador:** arm-none-eabi-gcc

## Como Executar
1. Clone o repositório:
    ```bash
    git clone https://github.com/Davileao10/PROJETO-FINAL.git
    ```
2. Configure o ambiente Pico SDK.
3. Compile o projeto:
    ```bash
    mkdir build
    cd build
    cmake ..
    make
    ```
4. Carregue o firmware:
    - Conecte o Raspberry Pi Pico W enquanto pressiona o botão BOOTSEL.
    - Copie o arquivo `.uf2` gerado para o dispositivo.
5. O Pico reiniciará automaticamente com o firmware.

## Resultados Esperados
- Contagem regressiva visual e sonora.
- Feedback sincronizado entre display, matriz de LEDs e buzzers.

## Referências
- [Pico SDK Documentation](https://pico-sdk.readthedocs.io)
- [Pico Examples](https://github.com/raspberrypi/pico-examples)

