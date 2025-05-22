#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);

const int ledVerde = 2;   // PRONTA
const int ledAmarelo = 4; // PENDENTE
const int ledVermelho = 7; // MANUTENÇÃO

String UID_VERDE = "49499d98";     // Cartão branco
String UID_AMARELO = "4ae1661a";   // Botão azul
String UID_VERMELHO = "133c7825";  // Terceiro cartão cadastrado


void setup() {
    Serial.begin(9600);
    SPI.begin();
    mfrc522.PCD_Init();

pinMode(ledVerde, OUTPUT);
pinMode(ledAmarelo, OUTPUT);
pinMode(ledVermelho, OUTPUT);


apagaTodosLEDs();
Serial.println("Aproxime o cartão ao leitor...");
}


void loop() {
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String uid = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        if (mfrc522.uid.uidByte[i] < 0x10) uid += "0";
        uid += String(mfrc522.uid.uidByte[i], HEX);
        }
    uid.toLowerCase();


    Serial.print("UID do cartão: ");
    Serial.println(uid);


    if (uid == UID_VERDE) {
        acenderLED(ledVerde);
        Serial.println("Status: PRONTA");
    } else if (uid == UID_AMARELO) {
        acenderLED(ledAmarelo);
        Serial.println("Status: PENDENTE");
    } else if (uid == UID_VERMELHO) {
        acenderLED(ledVermelho);
        Serial.println("Status: MANUTENCAO");
    } else {
        apagaTodosLEDs();
        Serial.println("Cartão não reconhecido!");
    }


    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    delay(1500);
    }
}


void acenderLED(int led) {
    apagaTodosLEDs();
    digitalWrite(led, HIGH);
}


void apagaTodosLEDs() {
    digitalWrite(ledVerde, LOW);
    digitalWrite(ledAmarelo, LOW);
    digitalWrite(ledVermelho, LOW);
}



