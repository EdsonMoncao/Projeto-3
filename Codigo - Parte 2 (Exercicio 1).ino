#include <ESP32Servo.h>

Servo meuServo;

const int pinoPotenciometro = 34; // Pino analógico (ADC1)
const int pinoServo = 13;         // Pino PWM para o servo

void setup() {
  Serial.begin(115200);
  
  // Aloca temporizadores para a biblioteca ESP32Servo
  ESP32PWM::allocateTimer(0);
  
  // Anexa o servo definindo pulsos mínimos e máximos
  meuServo.setPeriodHertz(50); // Frequência padrão
  meuServo.attach(pinoServo, 500, 2400);
}

void loop() {
  // Leitura do ADC da ESP32
  int leituraADC = analogRead(pinoPotenciometro);
  
  // Mapeia a leitura do potenciômetro para o ângulo do servomotor 
  int angulo = map(leituraADC, 0, 4095, 0, 180);
  
  // Aplica o ângulo ao motor
  meuServo.write(angulo);
  
  // Mapeamento extra para exibição do duty cycle proporcional 
  int porcentagemDuty = map(leituraADC, 0, 4095, 0, 100);
  
  Serial.print("Leitura ADC: "); Serial.print(leituraADC);
  Serial.print(" | Angulo: "); Serial.print(angulo); Serial.print("°");
  Serial.print(" | Duty Estimado: "); Serial.print(porcentagemDuty); Serial.println("%");
  
  delay(50);
}