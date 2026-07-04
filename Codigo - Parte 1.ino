// Definição dos pinos GPIO para o LED
const int pinoRed   = 13;
const int pinoGreen = 12;
const int pinoBlue  = 14;

// Parâmetros do PWM 
const int frequencia = 5000; // 5 kHz
const int resolucao  = 8;    // 8 bits

// Variáveis para armazenar o duty cycle atual de cada cor
int dutyRed   = 0;
int dutyGreen = 0;
int dutyBlue  = 0;

// Taxas de incremento independentes 
const int passoRed   = 15; // Incremento 3x maior
const int passoGreen = 5;  // Incremento base
const int passoBlue  = 10; // Incremento 2x maior

void setup() {
  Serial.begin(115200);
  
  // Configura os pinos com a frequência e resolução definidas
  ledcAttach(pinoRed, frequencia, resolucao);
  ledcAttach(pinoGreen, frequencia, resolucao);
  ledcAttach(pinoBlue, frequencia, resolucao);
  
  Serial.println("--- Sistema de Controle PWM LEDC Iniciado ---");
}

void loop() {
  // Aplica os valores de duty cycle aos pinos
  ledcWrite(pinoRed, dutyRed);
  ledcWrite(pinoGreen, dutyGreen);
  ledcWrite(pinoBlue, dutyBlue);

  // Envia as informações pelo monitor serial
  Serial.print("Red [Passo: "); Serial.print(passoRed); Serial.print(" | Duty: "); Serial.print(dutyRed); Serial.print("] \t");
  Serial.print("Green [Passo: "); Serial.print(passoGreen); Serial.print(" | Duty: "); Serial.print(dutyGreen); Serial.print("] \t");
  Serial.print("Blue [Passo: "); Serial.print(passoBlue); Serial.print(" | Duty: "); Serial.println(dutyBlue);

  // Incrementa os duty cycles
  dutyRed   += passoRed;
  dutyGreen += passoGreen;
  dutyBlue  += passoBlue;

  // Lógica de loop 
  if (dutyRed > 255)   dutyRed = 0;
  if (dutyGreen > 255) dutyGreen = 0;
  if (dutyBlue > 255)  dutyBlue = 0;

  delay(200); // Atraso para permitir a visualização no simulador e monitor serial
}
