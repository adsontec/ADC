## ADC 💾
Este projeto demonstra como usar o Raspberry Pi Pico W para interagir com um display SSD1306 por meio de um joystick usando um conversor analógico-digital que gera um pixel 8x8 centralizado no display, além de liga o led RGB usando PWM, com o objetivo de consolidar os conceitos sobre o uso de conversores analógico-digitais (ADC) no RP2040 e explorar as funcionalidades da placa de desenvolvimento BitDogLab.

## Funcionalidade: 📊
Ao iniciar o programa o pixel 8x8 fica centralizado no display ate que o joystick seja movimentado pelo usuario para ligar o LED RGB por meio do PWM, fazendo a conversão usando a função **adc_read()** para valores compatível com o tamanho do display usando a função **long map(long x, long in_min, long in_max, long out_min, long out_max)** para movimentar o pixel na tela.

## Requisitos de Hardware: 👨‍🔧
- Microcontrolador Raspberry Pi Pico W
- Display SSD1306
- Joystick
- LED RGB
- Botão A
  
## Requisitos de Software: ☑️
- Instalar o Visual Studio Code
- Instalar a extensão Wokwi no VSCode
- Instalar os pacotes do Pico SDK
  
## Pinos GPIO ⏰
- LED RGB: GPIOs (11, 12 e 13)
- Botão do Joystick: GPIO 22
- Joystick: GPIOs 26 e 27
- Botão A: GPIO 5
- Display SSD1306: (GPIO 14 e GPIO15)
  
## Explicação do Código: 🕵️
- **stdio_init_all()**: Inicializar comunicação padrão
- **adc_init()**: Inicializa o joystick
- **i2c_init(I2C_PORT, 400 * 1000)**: Inicializar comunicação I2C
- **init_display()**: Inicializar display
- **long map(long x, long in_min, long in_max, long out_min, long out_max)**: Função de conversão de valores 

## Link com a produção do vídeo 🎥
**🎬 Este é o vídeo do  projeto no [youtube](https://youtu.be/k9pU-2lRVI0)**

## Imagem do Projeto ✔️
![ADC](https://github.com/user-attachments/assets/5cbaa9d1-7e7e-419c-94c4-9a4fa2f0e69e)
