#include <stdio.h>             // Biblioteca padrão para entrada e saída, utilizada para printf.
#include "pico/stdlib.h"       // Biblioteca padrão para funções básicas do Pico, como GPIO e temporização.
#include "hardware/adc.h"      // Biblioteca para controle do ADC (Conversor Analógico-Digital).
#include "hardware/pwm.h"      // Biblioteca para controle do PWM
#include "inc/ssd1306.h"
#include "inc/font.h"

// Definições dos pinos para o joystick, botão e LEDs
#define VRX_PIN 26    // Define o pino GP26 para o eixo X do joystick (Canal ADC0).
#define VRY_PIN 27    // Define o pino GP27 para o eixo Y do joystick (Canal ADC1).
#define SW_PIN 22     // Define o pino GP22 para o botão do joystick (entrada digital).
#define LED1_PIN 11   // Define o pino GP11 para o LED 1.
#define LED2_PIN 13   // Define o pino GP13 para o LED 2.
#define LED3_PIN 12   // Define o pino GP12 para o LED controlado pelo botão (SW_PIN).
#define BUTTON_A 5    // Define o pino GP05 para o botão A.
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define PWM_GPIO 22
#define PWM_WRAP 20000

ssd1306_t ssd;
bool cor = true;

// Conexão dos pinos GPIO
const uint8_t GPIOs[] = {LED1_PIN, LED2_PIN, LED3_PIN, SW_PIN, BUTTON_A};

void init_gpio(){
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA); // Pull up the data line
    gpio_pull_up(I2C_SCL); // Pull up the clock line
}

void init_display(){
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd); // Configura o display
    ssd1306_send_data(&ssd); // Envia os dados para o display
  
    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}

// Função para inicializar saídas dos LEDs e a entrada do botão do joystick
void inicializar_GPIOs(){
    adc_gpio_init(VRX_PIN); // Configura GP26 (ADC0) para o eixo X do joystick.
    adc_gpio_init(VRY_PIN); // Configura GP27 (ADC1) para o eixo Y do joystick.

    for(int i = 0; i < 5; i++){
        gpio_init(GPIOs[i]);
        if( i < 3){
            gpio_set_dir(GPIOs[i], GPIO_OUT);
            gpio_put(GPIOs[i], 0);      // Inicialmente, LEDs apagado.
        }else{
            gpio_set_dir(GPIOs[i], GPIO_IN);
            gpio_pull_up(GPIOs[i]);     // Habilita o resistor pull-up interno
        }
    }
}

// Função de interrupção "É possivel observar o bounce do botão"
void gpio_irq_handler(uint gpio, uint32_t events){
    bool sw_value = gpio_get(LED3_PIN); // Obtém o estado atual
    gpio_put(LED3_PIN, !sw_value);      // Alterna o estado
    cor = !cor;
    // Atualiza o conteúdo do display com animações
    ssd1306_fill(&ssd, !cor); // Limpa o display
    ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor); // Desenha um retângulo    
    ssd1306_send_data(&ssd); // Atualiza o display
}

// Função map
long map(long x, long in_min, long in_max, long out_min, long out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int main() {
    stdio_init_all();
    adc_init();
    inicializar_GPIOs();
    i2c_init(I2C_PORT, 400 * 1000);
    init_gpio();
    init_display();

    // Configuração da interrupção com callback
    gpio_set_irq_enabled_with_callback(SW_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    // Loop infinito para ler continuamente os valores do joystick, do botão e controlar os LEDs.
    while (true) {        
        // Leitura do valor do ADC para VRX (Eixo X do joystick)
        adc_select_input(0); // Seleciona canal 0 (GP26 - VRX)
        uint16_t vrx_value = adc_read(); // Lê o valor do eixo X, de 0 a 4095.

        // Leitura do valor do ADC para VRY (Eixo Y do joystick)
        adc_select_input(1); // Seleciona canal 1 (GP27 - VRY)
        uint16_t vry_value = adc_read(); // Lê o valor do eixo Y, de 0 a 4095.

        //128×64 pixels
        int valx = vrx_value;
        valx = map(valx, 0, 4095, 3, 52); // Largura da tela menos 12 pixes

        int valy = vry_value;
        valy = map(valy, 0, 4095, 3, 116); // Comprimento da tela menos 12 pixes

        ssd1306_fill(&ssd, !cor);                           // Limpa o display 
        ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor);       // Desenha um retângulo  
        ssd1306_rect(&ssd, valx, valy, 8, 8, cor, cor);     // Desenha quandrado
        ssd1306_send_data(&ssd);                            // Atualiza o display 
        
        // Controle do LED1 baseado no valor do ADC0 (VRX)
        if (vrx_value > 2100) {
            gpio_put(LED1_PIN, 1); // Liga o LED1.
        } else {
            gpio_put(LED1_PIN, 0); // Desliga o LED1.
        }

        // Controle do LED2 baseado no valor do ADC1 (VRY)
        if (vry_value > 2100) {
            gpio_put(LED2_PIN, 1); // Liga o LED2.
        } else {
            gpio_put(LED2_PIN, 0); // Desliga o LED2.
        }

        if (gpio_get(BUTTON_A) == 1) {  // Verifica se o botão foi pressionado (nível baixo)
            gpio_put(LED3_PIN, 1);  // Acende o LED
        } else {
            gpio_put(LED1_PIN, 0);  // Apaga o LED
            gpio_put(LED2_PIN, 0);  // Apaga o LED
            gpio_put(LED3_PIN, 0);  // Apaga o LED
            sleep_ms(2000);
        }

        // Imprime os valores lidos na comunicação serial.
        printf("VRX: %u, VRY: %u\n", vrx_value, vry_value);

        // Introduz um atraso de 500 milissegundos antes de repetir a leitura.
        sleep_ms(500);
    }

    return 0;
}