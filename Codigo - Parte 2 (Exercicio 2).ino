#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "driver/mcpwm.h"

// Configuração do Display OLED (128x64 pixels)
#define LARGURA_OLED 128
#define ALTURA_OLED 64
Adafruit_SSD1306 display(LARGURA_OLED, ALTURA_OLED, &Wire, -1);

// Pinos do sistema
const int pinoMotorPWM = 19;
const int pinoBotaoParada = 4;

// Variáveis de controle do sistema
volatile bool paradaEmergencia = false;
float dutyCycleMotor = 60.0; // Começa com 60% de potência

// Rotina de Interrupção (ISR) para o botão de emergência
void IRAM_ATTR acionarParada() {
  paradaEmergencia = !paradaEmergencia; // Alterna o estado de parada
}

void setup() {
  Serial.begin(115200);
  
  // Configuração do botão com interrupção (Borda de subida/descida)
  pinMode(pinoBotaoParada, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinoBotaoParada), acionarParada, FALLING);

  // Inicialização do Display OLED via barramento I2C
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Falha ao inicializar o Display OLED SSD1306");
    for(;;);
  }
  display.clearDisplay();
  display.setTextColor(WHITE);

  // -------------------------------------------------------------
  // Configuração nativa do periférico MCPWM da ESP32
  // -------------------------------------------------------------
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, pinoMotorPWM);
  
  mcpwm_config_t pwm_config;
  pwm_config.frequency = 10000; // Frequência de 10 kHz para o motor
  pwm_config.cmpr_a = 0;        // Duty cycle inicial do canal A (0%)
  pwm_config.cmpr_b = 0;        // Canal B não utilizado
  pwm_config.counter_mode = MCPWM_UP_COUNTER;
  pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
  
  mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
  Serial.println("MCPWM Inicializado com sucesso.");
}

void loop() {
  float dutyAtual;

  if (paradaEmergencia) {
    dutyAtual = 0.0; // Zera a potência no estado de emergência
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, dutyAtual);
  } else {
    dutyAtual = dutyCycleMotor;
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, dutyAtual);
  }

  // Monitoramento via Serial
  Serial.print("Estado: "); Serial.print(paradaEmergencia ? "PARADO" : "RODANDO");
  Serial.print(" | Potência MCPWM: "); Serial.print(dutyAtual); Serial.println("%");

  // Atualização do Display OLED I2C
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("ESP32 - Controle MCPWM");
  display.drawLine(0, 10, 128, 10, WHITE);
  
  display.setCursor(0, 20);
  display.print("Status: ");
  display.println(paradaEmergencia ? "EMERGENCIA!" : "ATIVO");
  
  display.setCursor(0, 35);
  display.print("Duty Cycle: ");
  display.print(dutyAtual, 1);
  display.println(" %");
  
  display.display();

  delay(100);
}