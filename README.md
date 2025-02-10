# **Controle de LEDs e Display SSD1306 - Raspberry Pi Pico**

## **Descrição**

Este projeto foi desenvolvido para demonstrar o controle de LEDs RGB, Display SSD1306, matriz de LEDs WS2812 e exibição de informações no display, controlado por um Raspberry Pi Pico. O sistema permite o controle de LEDs e a exibição de mensagens no display OLED. Além disso, o projeto também inclui a funcionalidade de debouncing para botões de controle e comunicação via UART.

## **Componentes**

- 1x Raspberry Pi Pico
- 25x LEDs WS2812 (Matriz 5x5)
- 2x LEDs RGB (Verde e Azul)
- 2x Botões (Pushbutton A e B)
- 1x Display SSD1306 (OLED)
- 1x Resistor de 330Ω (para os LEDs RGB)

## **Requisitos do Sistema**

1. **Controles de LED com Botões:**
   - Botão A (GPIO 5) alterna o estado do LED verde.
   - Botão B (GPIO 6) alterna o estado do LED azul.
   - A alteração de estado é refletida no display OLED SSD1306, mostrando o status dos LEDs (ON/OFF).

2. **Controle de LEDs matriz:**
   - Os LEDs WS2812 (25 LEDs no total) podem ser controlados individualmente para exibir números de 0 a 9 em uma matriz 5x5, com a possibilidade de alterar as cores dos LEDs.

3. **Debouncing do Botão:**
   - A função de debouncing é implementada para evitar leituras incorretas devido ao "bouncing" dos botões.

4. **Exibição de Informações no Display OLED:**
   - O status dos LEDs (verde e azul) é exibido no display SSD1306. As mensagens de texto são atualizadas sempre que o estado dos LEDs mudar.

5. **Comunicação via UART:**
   - O sistema também pode receber caracteres via UART (porta serial) e exibir esses caracteres no display OLED.

6. **Sequenciamento de LEDs:**
   - Um número entre 0 e 9 pode ser exibido na matriz de LEDs WS2812. Cada número é representado por uma combinação única de LEDs acesos, que são configurados por arrays de índices de LEDs.

## **Funcionamento do Sistema**

- **Inicialização dos LEDs:**
  Ao iniciar o sistema, o Raspberry Pi Pico configura os LEDs WS2812, LEDs RGB (verde e azul), botões e o display SSD1306.

## **Instalação e Uso**

1. **Configuração do Hardware:**
   - Conecte os LEDs WS2812 ao GPIO 7 do Raspberry Pi Pico.
   - Conecte o LED verde ao GPIO 11 e o LED azul ao GPIO 12.
   - Conecte o botão A ao GPIO 5 e o botão B ao GPIO 6.
   - Conecte o display SSD1306 via I2C aos pinos GPIO 14 (SDA) e GPIO 15 (SCL).

2. **Carregue o Código:**
   - Compile o código fornecido para o Raspberry Pi Pico e carregue-o usando a interface USB.
   
3. **Interação com o Sistema:**
   - Pressione os botões para controlar o estado dos LEDs.
   - Envie caracteres via UART para visualizar no Display SSD1306 e os números na matriz WS2812.
   - O display OLED exibirá informações sobre o status dos LEDs.

## **Vídeo demonstrativo**
  - https://www.youtube.com/watch?v=cWMkah-MhBw
