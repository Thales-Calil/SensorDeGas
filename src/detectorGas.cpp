#define gasSensorPin 34
#define buzzerPin 22

const int limiteGas = 400;

void setup() {
  Serial.begin(115200);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  Serial.println("Aquecendo sensor MQ-6...");
  delay(60000);
  Serial.println("Sensor pronto para leitura.");
}

void loop() {
  int gasLevel = analogRead(gasSensorPin);
  Serial.print("Leitura do gás: ");
  Serial.println(gasLevel);

  if (gasLevel > limiteGas) {
    digitalWrite(buzzerPin, HIGH);
    Serial.println("!!! ALERTA: Gás detectado em alta concentração !!!");
  } else {
    digitalWrite(buzzerPin, LOW);
  }
  delay(2000);
} 