#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebServer.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

#define gasSensorPin 34 
#define buzzerPin 22
#define BOT_TOKEN "8096274270:AAEYWBbZG3m1T__nUwH-Pb26FifTXElL-kM"
#define CHAT_ID "2025820271"

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

unsigned long ultimaLeitura = 0;
const unsigned long intervalo = 1000;

const char* ssid_sta = "ROTEADOR_4";
const char* password_sta = "RoteadorCasa#4";

const int limiteGas = 400;

WebServer server(80);

int gasLevel = 0;
bool alarmeAtivado = false;
bool alertaEnviado = false; 

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

  html += "<p><em>Atualiza a cada 1 segundo...</em></p>";
  html += "<script>setTimeout(()=>{location.reload();}, 1000);</script>";
  html += "</body></html>";
  return html;
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 funcionando!");  

  secured_client.setInsecure();

  pinMode(buzzerPin, OUTPUT);

  WiFi.mode(WIFI_STA);
  delay(1000);

  WiFi.begin(ssid_sta, password_sta);
  Serial.print("Conectando ao WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado!");
  Serial.print("IP local: ");
  Serial.println(WiFi.localIP());

  // Configura página web
  server.on("/", []() {
    gasLevel = analogRead(gasSensorPin);
    alarmeAtivado = gasLevel > limiteGas;
    server.send(200, "text/html", gerarPaginaWeb());
  });

  server.begin();
  Serial.println("Servidor web iniciado.");

  bot.sendMessage(CHAT_ID, "Bot iniciado", "");
}

void loop() {
  server.handleClient();

  if (millis() - ultimaLeitura >= intervalo) {
    gasLevel = analogRead(gasSensorPin);
    Serial.print("Leitura do gás: ");
    Serial.println(gasLevel);

    if (gasLevel > limiteGas) {
      tone(buzzerPin, 2000);
      alarmeAtivado = true;

      if (!alertaEnviado) {
        Serial.println("!!! ALERTA: Gás detectado !!!");
        unsigned long inicio = millis();
        bool sucesso = bot.sendMessage(CHAT_ID, "🚨 GÁS DETECTADO!!", "");
        while (millis() - inicio < 3000) {
          server.handleClient();  // mantém a página web viva
          delay(10);
        }
        if (sucesso) {
          Serial.println("Mensagem enviada com sucesso.");
          alertaEnviado = true;
        } else {
          Serial.println("Erro ao enviar mensagem.");
        }
      }

    } else {
      noTone(buzzerPin);
      if (alarmeAtivado) {
        Serial.println("Gás voltou ao nível normal.");
        alertaEnviado = false;  // Permite envio no próximo pico
      }
      alarmeAtivado = false;
    }

    ultimaLeitura = millis();
  }
}