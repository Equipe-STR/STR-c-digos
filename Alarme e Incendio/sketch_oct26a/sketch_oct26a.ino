#include "WiFi.h"
#include "ESPAsyncWebServer.h"

#define PIN_SENSOR 14
#define PIN_BUZZER 17
#define PIN_FOGO 26

const char* ssid = "PROCURE O PETECA";
const char* password = "teste12345";

AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>
<body>
    <h1>TESTE</h1>
</body>
</html>)rawliteral";

void configureIOPin(){
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_SENSOR, INPUT);
  pinMode(PIN_FOGO, INPUT);
}

void acionaBuzzer(){
  for (int i = 00; i <= 10; i += 1) {
    tone(PIN_BUZZER, 1000);
    delay(300);
    tone(PIN_BUZZER, 500);
    delay(300);
  }
}

void setup() {
  ledcSetup(1, 2, 3);
  // Para corrigir um bug
  configureIOPin();

  Serial.begin(9600);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.print("IP para conectar: ");
  Serial.println(WiFi.localIP());
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  server.begin();
}

void loop() {
  int sinal = digitalRead(PIN_SENSOR);
  bool leituraFogo = digitalRead(PIN_FOGO);
  Serial.println(leituraFogo);
  if(sinal == HIGH){
    acionaBuzzer();
  }
  else if(leituraFogo==HIGH){
    acionaBuzzer();
  }
  //LOW: Nada detectado
  else{
    // Desativa o buzzer
    noTone(PIN_BUZZER);
  }
}
