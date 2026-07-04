# Projeto 3: Controle PWM e Comunicação (SEL0433)

**Disciplina:** SEL0433 - Aplicação de Microprocessadores  
**Autor:** Edson Rodrigues Monção Neto
**Número USP:** 14566780

---

## Objetivos do Projeto
Este repositório possui os códigos-fonte desenvolvidos para o Projeto 3 da disciplina SEL0433. O objetivo principal foi exercitar a programação de microcontroladores de 32 bits (ESP32 DevKit) explorando conceitos de Modulação por Largura de Pulso (PWM), comunicação serial (I2C/UART) e interrupções de hardware por meio de simulações no ambiente virtual Wokwi.

---

## Parte 1: Controle PWM de LED RGB (LEDC)

Nesta etapa, implementou-se o controle de intensidade luminosa de um LED RGB de catodo comum utilizando a biblioteca nativa `LEDC` da ESP32.

### Conceitos e Implementação
O sistema opera com uma frequência de 5 kHz e resolução de 8 bits (duty cycle variando de 0 a 255). A lógica desenvolvida permite o incremento contínuo e independente das cores:

### Bibliotecas Empregadas
* **Nativa (Core da ESP32):** Foram utilizadas as funções `ledcAttach()` e `ledcWrite()` diretamente, sem a necessidade de bibliotecas externas adicionais, aproveitando a API moderna do Arduino Core v3.0+.

### Trecho de Código Principal
Abaixo, o destaque para a configuração do canal PWM e a atribuição dinâmica do duty cycle no loop principal:

```
// Configura os pinos com a frequência e resolução definidas
  ledcAttach(pinoRed, frequencia, resolucao);
  ledcAttach(pinoGreen, frequencia, resolucao);
  ledcAttach(pinoBlue, frequencia, resolucao);
  
  // ... (dentro do loop) ...
  
  // Aplica os valores de duty cycle aos pinos
  ledcWrite(pinoRed, dutyRed);
  ledcWrite(pinoGreen, dutyGreen);
  ledcWrite(pinoBlue, dutyBlue);

  // Incrementa os duty cycles
  dutyRed   += passoRed;
  dutyGreen += passoGreen;
  dutyBlue  += passoBlue;

  // Lógica de loop (reinicia em 0 ao ultrapassar o limite de 8 bits)
  if (dutyRed > 255)   dutyRed = 0;
  if (dutyGreen > 255) dutyGreen = 0;
  if (dutyBlue > 255)  dutyBlue = 0;
```
## Parte 2: Controle de Motores e Periféricos
A segunda parte foca em aplicações mais avançadas de PWM para o acionamento de atuadores, dividida em dois exercícios.

### Exercício 1: Servomotor e ADC
Controle de posição de um servomotor utilizando um sinal analógico proveniente de um potenciômetro (ADC).
 - Bibliotecas: Utilizou-se a biblioteca externa ESP32Servo para compatibilizar a geração de pulsos de controle do servo com a arquitetura de timers da ESP32.
 - Lógica: O valor lido pelo ADC no pino 34 (resolução de 12 bits, 0-4095) foi mapeado para a faixa de operação do servo (0 a 180 graus).

### Trecho de Código Principal do Exercício 1:
```
#include <ESP32Servo.h>

Servo meuServo;
const int pinoPotenciometro = 33; // ADC para leitura da tensão 
const int pinoServo = 13;         // Saída PWM para o motor 

void setup() {
  // Aloca temporizadores de hardware da ESP32 para a biblioteca ESP32Servo
  ESP32PWM::allocateTimer(0);
  
  // Configura a frequência padrão do servo (50 Hz) e anexa ao pino
  meuServo.setPeriodHertz(50); 
  meuServo.attach(pinoServo, 500, 2400);
}

void loop() {
  // Lê o sinal analógico do potenciômetro
  int leituraADC = analogRead(pinoPotenciometro);
  
  // Converte o valor lido no ADC para um ângulo de 0 a 180 graus 
  int angulo = map(leituraADC, 0, 4095, 0, 180);
  
  // Atualiza a posição do servomotor
  meuServo.write(angulo);
  
  delay(50);
}
```
### Exercício 2: Aplicação Livre (Motor DC, MCPWM e OLED)
Sistema focado no controle de um Motor DC utilizando o periférico de hardware MCPWM (Motor Control PWM) da ESP32, integrado a um display I2C e uma interrupção de emergência. 
 - Bibliotecas: Utilizou-se as bibliotecas Adafruit GFX Library e Adafruit SSD1306.
 - Conceitos: O sinal PWM foi gerado nativamente no pino 19, que está conectado a um LED para a visualização do funcionamento. Uma interrupção externa (hardware interrupt) foi configurada via attachInterrupt no pino 4 para acionar um modo de emergência, zerando o duty cycle. Os dados (status do motor e potência) são monitorados via I2C no display OLED.
   
### Trecho de Código Principal do Exercício 2:
```
// Rotina de Interrupção (ISR) para o botão de emergência
void IRAM_ATTR acionarParada() {
  paradaEmergencia = !paradaEmergencia; 
}

// ... (dentro do setup) ...

  // Configuração do botão com interrupção 
  pinMode(pinoBotaoParada, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinoBotaoParada), acionarParada, FALLING);

  // Configuração nativa do periférico MCPWM
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, pinoMotorPWM);
  
  mcpwm_config_t pwm_config;
  pwm_config.frequency = 10000; // Frequência de 10 kHz
  pwm_config.cmpr_a = 0;        
  pwm_config.cmpr_b = 0;        
  pwm_config.counter_mode = MCPWM_UP_COUNTER;
  pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
  
  mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
```

### Discussão de Resultados
O controle das cores do LED funcionou conforme esperado pelo monitor serial. No exercício 2, a interrupção parou o PWM instantaneamente, provando que o código responde bem a emergências.
Usar o ESP32 só para acender um LED é exagero e custa caro. Isso vale a pena quando o projeto tem muitas coisas ao mesmo tempo, como nesse projeto onde usamos tela OLED (I2C) e interrupções de hardware juntas, além da possibilidade de usar Wi-Fi/Bluetooth no futuro.
