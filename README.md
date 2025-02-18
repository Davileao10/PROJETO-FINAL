# Controle de Joystick com RP2040 e Display SSD1306

- **Código-fonte** (pwm_irq.c).
- **Vídeo de demonstração**:
   - [https://drive.google.com/file/d/1jQv01XCyHK3hzGYLnKJrzjA0B7Lp9td0/view?usp=drivesdk ]


Este projeto implementa um sistema de controle baseado em joystick utilizando o microcontrolador RP2040 na placa BitDogLab. Ele envolve a leitura de valores analógicos via ADC, controle de LEDs via PWM e exibição de um quadrado móvel no display OLED SSD1306, seguindo os requisitos da atividade prática.

## 📌 Funcionalidades Implementadas

- **Leitura do joystick (ADC)**: Captação dos valores dos eixos X e Y para controle dos LEDs e do display.
- **Controle de LEDs RGB (PWM)**:
  - LED Azul: Brilho controlado pelo eixo Y.
  - LED Vermelho: Brilho controlado pelo eixo X.
- **Movimentação do quadrado no display OLED**:
  - O quadrado de 8x8 pixels se move proporcionalmente aos valores do joystick.
- **Alteração do estilo da borda do display**:
  - Sem borda → Borda fina → Borda grossa (alternado pelo botão do joystick).
- **Alternância do LED verde**: Cada vez que o botão do joystick é pressionado, o LED verde liga/desliga.
- **Ativação/Desativação dos LEDs PWM**: O botão A liga ou desliga a variação dos LEDs RGB.
- **Uso de interrupções (IRQ)**: Implementado para os botões do joystick e botão A.

## 🛠️ Componentes Utilizados

- **RP2040 (Placa BitDogLab)**
- **Joystick** (conectado aos GPIOs 26 e 27)
- **Botão do Joystick** (GPIO 22)
- **Botão A** (GPIO 5)
- **LED RGB** (GPIOs 11, 12 e 13)
- **Display SSD1306** (comunicação via I2C: SDA no GPIO 14, SCL no GPIO 15)

## 📜 Requisitos Atendidos
✅ Leitura do joystick via ADC.  
✅ Controle de LEDs RGB via PWM.  
✅ Exibição gráfica no display via I2C.  
✅ Uso de interrupções para os botões.  
✅ Alternância de estilos de borda no display.  
✅ Alternância do estado do LED verde pelo botão do joystick.  
✅ Ativação/desativação dos LEDs PWM pelo botão A.  

## 🔧 Como Rodar o Projeto

1. **Configurar o ambiente de desenvolvimento**:
   - Instale o **SDK do Raspberry Pi Pico**.
   - Configure o **CMake** e o **compilador arm-none-eabi-gcc**.

2. **Compilar e carregar o código**:
   ```sh
   mkdir build
   cd build
   cmake ..
   make
   ```
   - Copie o arquivo `.uf2` gerado para o Pico (modo bootloader).

3. **Executar o código**:
   - Conecte o RP2040 e observe o funcionamento do joystick, LEDs e display.



