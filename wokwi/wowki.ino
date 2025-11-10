#include <WiFi.h>
#include <HTTPClient.h>

const char* WIFI_SSID  = "Wokwi-GUEST";
const char* WIFI_PASS  = "";

const char* TS_SERVER  = "http://api.thingspeak.com/update";
const char* TS_API_KEY = "RVFSGCR8NTA1RSII";

const int LED_VERDE      = 23;
const int LED_AMARELO    = 22;
const int LED_VERMELHO   = 21;

int motoId      = 1;
int estadoAtual = 0;

struct StatusInfo {
  int codigo;
  const char* texto;
  int ledPin;
};

StatusInfo statuses[] = {
  {0, "DESLIGADO",  -1},
  {1, "PRONTA",     LED_VERDE},
  {2, "PENDENTE",   LED_AMARELO},
  {3, "MANUTENCAO", LED_VERMELHO}
};

struct EventoDia {
  unsigned long durMs;
  int statusCodigo;
};

EventoDia roteiroDia[] = {
  { 30000, 1 },
  { 45000, 2 },
  { 30000, 3 },
  { 45000, 1 }
};

const int QTD_EVENTOS = sizeof(roteiroDia) / sizeof(roteiroDia[0]);

int eventoAtual = 0;
unsigned long inicioEventoMs = 0;

void apagarLeds() {
  digitalWrite(LED_VERDE,   LOW);
  digitalWrite(LED_AMARELO, LOW);
  digitalWrite(LED_VERMELHO,LOW);
}

void conectarWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;

  Serial.print("Conectando WiFi: ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  int tentativas = 0;

  while (WiFi.status() != WL_CONNECTED && tentativas < 40) {
    delay(250);
    Serial.print(".");
    tentativas++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi conectado!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFalha ao conectar WiFi");
  }
}

void enviarThingSpeak(int statusCodigo) {
  if (WiFi.status() != WL_CONNECTED) {
    conectarWiFi();
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Sem WiFi, nao envia para ThingSpeak.");
    return;
  }

  int patio = (statusCodigo == 1) ? 1 : 0;

  HTTPClient http;
  http.setConnectTimeout(5000);

  String url = String(TS_SERVER);
  url += "?api_key=";
  url += TS_API_KEY;
  url += "&field2=";
  url += patio;

  Serial.print("Enviando para field2: ");
  Serial.println(url);

  http.begin(url);
  int httpResponseCode = http.GET();

  if (httpResponseCode == 200) {
    Serial.println("✓ Enviado com sucesso!");
    String payload = http.getString();
    Serial.print("Resposta: ");
    Serial.println(payload);
  } else {
    Serial.print("✗ Erro HTTP: ");
    Serial.println(httpResponseCode);
  }

  http.end();
  delay(2000);  // Aguarda 2 segundos antes de próximo envio
}

void aplicarStatus(int novoStatus) {
  estadoAtual = novoStatus;

  apagarLeds();
  if (statuses[estadoAtual].ledPin != -1) {
    digitalWrite(statuses[estadoAtual].ledPin, HIGH);
  }

  Serial.print("Moto ");
  Serial.print(motoId);
  Serial.print(" -> Status: ");
  Serial.println(statuses[estadoAtual].texto);

  enviarThingSpeak(estadoAtual);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_VERDE,   OUTPUT);
  pinMode(LED_AMARELO, OUTPUT);
  pinMode(LED_VERMELHO,OUTPUT);

  apagarLeds();

  Serial.println("Iniciando ESP32 EasyMoto IoT - Simulacao Dia Moto 1...");
  conectarWiFi();

  eventoAtual    = 0;
  inicioEventoMs = millis();
  aplicarStatus(roteiroDia[eventoAtual].statusCodigo);
}

void loop() {
  unsigned long agora    = millis();
  unsigned long durAtual = roteiroDia[eventoAtual].durMs;

  if (agora - inicioEventoMs >= durAtual) {
    eventoAtual++;
    if (eventoAtual >= QTD_EVENTOS) {
      eventoAtual = 0;
    }

    inicioEventoMs = agora;
    aplicarStatus(roteiroDia[eventoAtual].statusCodigo);
  }

  delay(100);
}