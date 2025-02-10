#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "hardware/pio.h"
#include "ws2812.pio.h"
#include "inc/ssd1306.h"
#include "inc/font.h"

#define endereco 0x3C  // Endereço I2C do display SSD1306

// I2C defines
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15

// Botões (os pinos conectados aos botões A e B)
#define BOTAO_A_PIN  5
#define BOTAO_B_PIN  6

// Definição dos LEDs RGB (utilizando GPIO 11 e 12 para controlar os LEDs verde e azul)
#define LED_VERDE_PIN 11
#define LED_AZUL_PIN 12

#define IS_RGBW false
#define NUM_PIXELS 25
#define WS2812_PIN 7

// Variáveis globais para controle do LED (matriz) e cores
uint8_t selected_led = 1; // Índice do LED a ser controlado (0 a 24)
uint8_t selected_r = 20;   // Intensidade do vermelho (0 a 255)
uint8_t selected_g = 0;   // Intensidade do verde (0 a 255)
uint8_t selected_b = 0;   // Intensidade do azul (0 a 255)

// Variáveis globais
static uint32_t last_time = 0; // Armazena o tempo do último evento (em microssegundos)

bool led_verde_estado = false;  // Variável para controlar o estado do LED verde
bool led_azul_estado = false;  // Variável para controlar o estado do LED azul

ssd1306_t ssd;

// Definição dos arrays que representam os LEDs de cada número (com base em uma matriz 5x5)
int num_0[] = {1, 2, 3, 6, 8, 11, 13, 16, 18, 21, 22, 23};  
int num_1[] = {2, 7, 12, 16, 17, 22};   
int num_2[] = {1, 2, 3, 6, 11, 12, 13, 18, 21, 22, 23};       
int num_3[] = {1, 2, 3, 8, 11, 12, 13, 18, 21, 22, 23};     
int num_4[] = {1, 8, 11, 12, 13, 16, 18, 21, 23}; 
int num_5[] = {1, 2, 3, 8, 11, 12, 13, 16, 21, 22, 23};  
int num_6[] = {1, 2, 3, 6, 8, 11, 12, 13, 16, 21, 22, 23}; 
int num_7[] = {2, 7, 12, 18, 21, 22, 23}; 
int num_8[] = {1, 2, 3, 6, 8, 11, 12, 13, 16, 18, 21, 22, 23}; 
int num_9[] = {1, 2, 3, 8, 11, 12, 13, 16, 18, 21, 22, 23};  

// Ponteiros para os arrays de LEDs que representam números
int* num[] = {num_0, num_1, num_2, num_3, num_4, num_5, num_6, num_7, num_8, num_9};

// Tamanhos individuais de cada array (quantos LEDs são acesos por cada número)
int tamanhos[] = {12, 6, 11, 11, 9, 11, 12, 7, 13, 12};  

int array_atual = 0; // Índice do número atual sendo exibido

int ler_caractere_uart() {
    if (stdio_usb_connected()) {  // Verifica se há dados disponíveis no UART0
        return getchar_timeout_us(0);  // Lê o caractere do UART0
    }
    return -1;  // Retorna -1 se não houver dados
}

// Função para enviar um valor para o pino de controle do LED
static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

// Função para gerar um valor de cor (R, G, B)
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

// Buffer para armazenar os estados de todos os LEDs
uint32_t led_buffer[NUM_PIXELS] = {0}; 

// Função para atualizar o buffer de LEDs com base no número atual
void update_led_buffer() {
    // Apaga todos os LEDs
    for (int i = 0; i < NUM_PIXELS; i++) {
        led_buffer[i] = 0; // Desliga todos os LEDs
    }

    // Atualiza os LEDs com base no número selecionado
    for (int i = 0; i < tamanhos[array_atual]; i++) {
        led_buffer[num[array_atual][i]] = urgb_u32(selected_r, selected_g, selected_b);
    }
}

// Função para enviar os estados do buffer para os LEDs
void set_leds_from_buffer() {
    for (int i = 0; i < NUM_PIXELS; i++) {
        put_pixel(led_buffer[i]);
    }
}

// Função para exibir o número na matriz WS2812
void exibir_numero_ws2812(char numero) {
    // Converte o caractere em número
    int num_index = numero - '0';  // Converte o caractere ('0' a '9') para o índice do número (0 a 9)

    // Atualiza o buffer de LEDs com base no número selecionado
    array_atual = num_index;  // Atualiza o índice do número atual a ser exibido
    update_led_buffer();  // Atualiza o buffer de LEDs com os LEDs correspondentes ao número
    set_leds_from_buffer();  // Envia os estados dos LEDs para a matriz WS2812
}

void gpio_irq_handler(uint gpio, uint32_t events)
{
    // Obtém o tempo atual em microssegundos
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    
    // Verifica se passou tempo suficiente desde o último evento (debouncing)
    if (current_time - last_time > 200000) // 200 ms de debouncing
    {
        last_time = current_time; // Atualiza o tempo do último evento

        // Botão A: Incrementa o índice do número exibido
        if (gpio == 5) {  // Botão 0
            led_verde_estado = !led_verde_estado;  // Inverte o estado do LED verde
            gpio_put(LED_VERDE_PIN, led_verde_estado);  // Atualiza o LED verde

            // Exibe o estado do LED verde no display
            ssd1306_fill(&ssd, false);  // Limpa o display
            ssd1306_draw_string(&ssd, led_verde_estado ? "LED Verde: ON" : "LED Verde: OFF", 8, 10);  // Exibe o texto
            ssd1306_send_data(&ssd);  // Envia os dados para o display
        }
        
        // Botão B: Decrementa o índice do número exibido
        if (gpio == 6) {  // Botão 1
            led_azul_estado = !led_azul_estado;  // Inverte o estado do LED azul
            gpio_put(LED_AZUL_PIN, led_azul_estado);  // Atualiza o LED azul

            // Exibe o estado do LED azul no display
            ssd1306_fill(&ssd, false);  // Limpa o display
            ssd1306_draw_string(&ssd, led_azul_estado ? "LED Azul: ON" : "LED Azul: OFF", 8, 10);  // Exibe o texto
            ssd1306_send_data(&ssd);  // Envia os dados para o display
        }

    }
}

int main() {
    stdio_init_all();

    uart_init(uart0, 115200);  // Inicializa o UART0
    gpio_set_function(0, GPIO_FUNC_UART); // GPIO 0 como RX
    gpio_set_function(1, GPIO_FUNC_UART); // GPIO 1 como TX

    // Inicialização do I2C e do display SSD1306
    i2c_init(I2C_PORT, 400 * 1000);  // Inicializa o I2C com taxa de 400 kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);  // Configura o pino SDA para I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);  // Configura o pino SCL para I2C
    gpio_pull_up(I2C_SDA);  // Ativa o pull-up no pino SDA
    gpio_pull_up(I2C_SCL);  // Ativa o pull-up no pino SCL

    
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);  // Inicializa o display SSD1306
    ssd1306_config(&ssd);  // Configura o display
    ssd1306_send_data(&ssd);  // Envia os dados de configuração para o display
    ssd1306_fill(&ssd, false);  // Limpa o display
    ssd1306_send_data(&ssd);  // Envia os dados de limpeza para o display

    // Inicialização dos LEDs
    gpio_init(LED_VERDE_PIN);  // Inicializa o pino do LED verde
    gpio_set_dir(LED_VERDE_PIN, GPIO_OUT);  // Define o pino do LED verde como saída
    gpio_init(LED_AZUL_PIN);  // Inicializa o pino do LED azul
    gpio_set_dir(LED_AZUL_PIN, GPIO_OUT);  // Define o pino do LED azul como saída

    // Inicialização dos botões
    gpio_init(BOTAO_A_PIN);  // Inicializa o pino do botão A
    gpio_set_dir(BOTAO_A_PIN, GPIO_IN);  // Define o pino do botão A como entrada
    gpio_pull_up(BOTAO_A_PIN);  // Ativa o pull-up no botão A

    gpio_init(BOTAO_B_PIN);  // Inicializa o pino do botão B
    gpio_set_dir(BOTAO_B_PIN, GPIO_IN);  // Define o pino do botão B como entrada
    gpio_pull_up(BOTAO_B_PIN);  // Ativa o pull-up no botão B

    // Configuração da interrupção com callback para os botões
    gpio_set_irq_enabled_with_callback(BOTAO_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BOTAO_B_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    // Inicializa o PIO para controle dos LEDs WS2812
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    // Inicializa o programa para os LEDs WS2812
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);
    
    char buffer_char = 0;  // Variável para armazenar o caractere recebido via UART

    while (true) {
        int caractere = getchar_timeout_us(0);

        // Verifica se há dados no UART para ler
        if (caractere != PICO_ERROR_TIMEOUT) {
            buffer_char = (char)caractere; // Atualiza a variável com o caractere recebido

            // Exibe o caractere recebido no display SSD1306
            ssd1306_fill(&ssd, false);  // Limpa o display
            ssd1306_rect(&ssd, 4, 54, 20, 20, true, false); // Desenha um retângulo
            ssd1306_draw_char(&ssd, buffer_char, 60, 10);  // Desenha o caractere na posição (8,10)
            ssd1306_draw_string(&ssd, "HASS", 50, 48); // Desenha uma string 
            ssd1306_send_data(&ssd);  // Envia os dados para atualizar o display

            // Se o caractere for um número, exibe na matriz WS2812
            if (caractere >= '0' && caractere <= '9') {
                // Exibe o número na matriz de LEDs WS2812
                exibir_numero_ws2812(buffer_char);
            }
        }

        sleep_ms(50);
    }

}