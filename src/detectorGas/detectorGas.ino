#include <WiFi.h>
#include <WebServer.h>

#define gasSensorPin 34 
#define buzzerPin 22

unsigned long ultimaLeitura = 0;
const unsigned long intervalo = 1000;

const int limiteGas = 400;

WebServer server(80);

int gasLevel = 5;
bool alarmeAtivado = false;

String gerarPaginaWeb() {
  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<title>Monitor de Gás ESP32</title>";
  html += "<style>body{font-family:Arial;text-align:center;} .alerta{color:red;font-weight:bold;}</style>";
  html += "</head><body>";
  html += "<h2>Monitor de Gás - ESP32</h2>";
  html += "<p>Leitura atual do gás: <strong>" + String(gasLevel) + "</strong></p>";

  if (alarmeAtivado) {
    html += "<p class='alerta'>!!! ALERTA: Gás detectado em alta concentração !!!</p>";
  } else {
    html += "<p>Status: Normal</p>";
  }

  html += "<p><em>Atualiza em 5 segundos...</em></p>";
  html += "<script>setTimeout(()=>{location.reload();}, 5000);</script>";
  html += "</body></html>";
  return html;
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 funcionando!");

  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  // Cria uma rede Wi-Fi própria (Access Point)
  WiFi.softAP("ESP32-GAS", "12345678"); // Nome da rede e senha
  delay(1000); // Pequena espera para estabilidade

  IPAddress ip = WiFi.softAPIP();
  Serial.print("IP do ESP32 (AP): ");
  Serial.println(ip);

  // Configura a página web
  server.on("/", []() {
    gasLevel = analogRead(gasSensorPin);
    alarmeAtivado = gasLevel > limiteGas;
    server.send(200, "text/html", gerarPaginaWeb());
  });

  server.begin();
  Serial.println("Servidor web iniciado.");
}

void loop() {
  server.handleClient();

  if (millis() - ultimaLeitura >= intervalo) {
    gasLevel = analogRead(gasSensorPin);
    Serial.print("Leitura do gás: ");
    Serial.println(gasLevel);

    if (gasLevel > limiteGas) {
      digitalWrite(buzzerPin, HIGH);
      alarmeAtivado = true;
      Serial.println("!!! ALERTA: Gás detectado !!!");
    } else {
      digitalWrite(buzzerPin, LOW);
      alarmeAtivado = false;
    }

    ultimaLeitura = millis();
  }
}
