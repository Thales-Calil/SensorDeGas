#include <WiFi.h>
#include <WebServer.h>

#define gasSensorPin 34 
#define buzzerPin 22

const int limiteGas = 400;

const char* ssid = "SEU_SSID";
const char* password = "SUA_SENHA";

WebServer server(80);

int gasLevel = 0;
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
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  Serial.println("Conectando ao Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectado!");
  Serial.print("IP do ESP32: ");
  Serial.println(WiFi.localIP());

  server.on("/", []() {
    server.send(200, "text/html", gerarPaginaWeb());
  });

  server.begin();
  Serial.println("Servidor web iniciado.");
}

void loop() {
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

  server.handleClient();
  delay(1000);
}
