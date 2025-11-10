#include <WiFi.h>
#include <PubSubClient.h>
#include <time.h>

const char* WIFI_SSID = "nomedasuarede";
const char* WIFI_PASS = "suasenhadewifi";
const char* BROKER    = "broker.hivemq.com";
const uint16_t BROKER_PORT = 1883;
const char* DEVICE_ID = "moto-01";

const int LED_VERDE    = 23;
const int LED_AMARELO  = 22;
const int LED_VERMELHO = 21;

const int BTN_VERDE    = 26;
const int BTN_AMARELO  = 27;
const int BTN_VERMELHO = 25;

struct Btn {
  int pin;
  bool lastStable = HIGH;
  bool lastRead   = HIGH;
  unsigned long lastChange = 0;
};
Btn bverde{BTN_VERDE}, bamarelo{BTN_AMARELO}, bvermelho{BTN_VERMELHO};
const unsigned long DEBOUNCE_MS = 40;

enum Status {PRONTA, PENDENTE, MANUTENCAO};
Status estadoAtual = PENDENTE;

WiFiClient espClient;
PubSubClient mqtt(espClient);

String topicStatus() {
  return String("mottu/bike/status/") + DEVICE_ID;
}

const char* statusToStr(Status s){
  switch (s){
    case PRONTA: return "PRONTA";
    case PENDENTE: return "PENDENTE";
    default: return "MANUTENCAO";
  }
}

void setLeds(Status s){
  digitalWrite(LED_VERDE,    s==PRONTA);
  digitalWrite(LED_AMARELO,  s==PENDENTE);
  digitalWrite(LED_VERMELHO, s==MANUTENCAO);
}

String isoNowUTC(){
  time_t now; time(&now);
  struct tm t; gmtime_r(&now, &t);
  char buf[25];
  strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &t);
  return String(buf);
}

void publica(Status s){
  if (!mqtt.connected()) return;
  String payload = String("{\"id\":\"") + DEVICE_ID +
                   "\",\"status\":\"" + statusToStr(s) +
                   "\",\"ts\":\"" + isoNowUTC() +
                   "\",\"uptime_ms\":" + String(millis()) + "}";
  mqtt.publish(topicStatus().c_str(), payload.c_str(), true);
  Serial.print("[MQTT] "); Serial.print(topicStatus()); Serial.print(" <- "); Serial.println(payload);
}

bool debouncePress(Btn &b){
  bool reading = digitalRead(b.pin);
  if (reading != b.lastRead){
    b.lastChange = millis();
    b.lastRead = reading;
  }
  if ((millis() - b.lastChange) > DEBOUNCE_MS){
    if (reading != b.lastStable){
      b.lastStable = reading;
      if (reading == LOW) return true;
    }
  }
  return false;
}

void handlePress(Status novo){
  if (estadoAtual == novo) return;
  estadoAtual = novo;
  setLeds(novo);
  publica(novo);
}

void wifiConnectOnce(uint32_t timeout_ms=10000){
  if (WiFi.status() == WL_CONNECTED) return;
  Serial.print("[WiFi] Conectando a "); Serial.println(WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  uint32_t t0 = millis();
  while (WiFi.status()!=WL_CONNECTED && (millis()-t0) < timeout_ms){
    delay(200);
  }
  if (WiFi.status()==WL_CONNECTED){
    Serial.print("[WiFi] OK: "); Serial.println(WiFi.localIP());
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  } else {
    Serial.println("[WiFi] Falhou (offline).");
  }
}

void mqttConnectOnce(){
  if (WiFi.status()!=WL_CONNECTED || mqtt.connected()) return;
  mqtt.setServer(BROKER, BROKER_PORT);
  String cid = String("esp32-") + String((uint32_t)ESP.getEfuseMac(), HEX);
  String willTopic = String("mottu/bike/connection/") + DEVICE_ID;
  const char* willMsg = "offline";
  bool ok = mqtt.connect(cid.c_str(), NULL, NULL, willTopic.c_str(), 0, true, willMsg);
  if (ok){
    Serial.println("[MQTT] Conectado");
    mqtt.publish(willTopic.c_str(), "online", true);
    publica(estadoAtual);
  } else {
    Serial.print("[MQTT] Falha connect, rc="); Serial.println(mqtt.state());
  }
}

uint32_t lastReconnectTry = 0;
const uint32_t RECONNECT_EVERY_MS = 3000;

void setup(){
  Serial.begin(115200);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AMARELO, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(BTN_VERDE, INPUT_PULLUP);
  pinMode(BTN_AMARELO, INPUT_PULLUP);
  pinMode(BTN_VERMELHO, INPUT_PULLUP);
  setLeds(estadoAtual);
  wifiConnectOnce();
  mqttConnectOnce();
  publica(estadoAtual);
}

void loop(){
  if (WiFi.status()!=WL_CONNECTED){
    if (millis() - lastReconnectTry > RECONNECT_EVERY_MS){
      lastReconnectTry = millis();
      wifiConnectOnce(3000);
    }
  } else {
    if (!mqtt.connected()){
      if (millis() - lastReconnectTry > RECONNECT_EVERY_MS){
        lastReconnectTry = millis();
        mqttConnectOnce();
      }
    } else {
      mqtt.loop();
    }
  }
  if (debouncePress(bverde))    handlePress(PRONTA);
  if (debouncePress(bamarelo))  handlePress(PENDENTE);
  if (debouncePress(bvermelho)) handlePress(MANUTENCAO);
  delay(5);
}
