#include <WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>

// Wi-Fi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// MQTT ThingsBoard
const char* mqtt_server = "mqtt.thingsboard.cloud";
const int mqtt_port = 1883;
const char* mqtt_username = "JUAcqz8Sp52UcNl7FlGA"; // Token do device
const char* mqtt_password = ""; // Senha vazia

WiFiClient espClient;
PubSubClient client(espClient);

// Pinos
const int buttonPin = 2;
const int relayPin = 4;
const int ledValvePin = 5;

// Variáveis
bool valvulaFechada = false;
float consumoLitros = 0;
float limiteLitros = 110.0;
float bonusLitros = 20.0;
int bonusLiberadoContador = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);

const float ml_per_intervalo = 10000.0; // Simula 10L/s

// Botão - debounce
unsigned long lastButtonPress = 0;
const unsigned long debounceDelay = 300;
bool botaoPressionadoAnterior = HIGH;

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(relayPin, OUTPUT);
  pinMode(ledValvePin, OUTPUT);

  digitalWrite(relayPin, LOW);
  digitalWrite(ledValvePin, LOW);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Iniciando...");

  WiFi.begin(ssid, password);
  Serial.print("Conectando Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado!");

  client.setServer(mqtt_server, mqtt_port);

  reconnect();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi + MQTT OK");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Consumo: 0.00L");
  lcd.setCursor(0, 1);
  lcd.print("Fluxo: 10.0L/s");
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Tentando conectar ao MQTT...");
    if (client.connect("ESP32Client", mqtt_username, mqtt_password)) {
      Serial.println("Conectado ao MQTT!");
    } else {
      Serial.print("Falha MQTT, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente...");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  static unsigned long lastMeasure = 0;
  unsigned long now = millis();

  // Leitura do botão
  bool estadoBotaoAtual = digitalRead(buttonPin);
  if (estadoBotaoAtual == LOW && botaoPressionadoAnterior == HIGH && (now - lastButtonPress > debounceDelay)) {
    lastButtonPress = now;
    botaoPressionadoAnterior = LOW;

    if (valvulaFechada) {
      bonusLiberadoContador++;
      limiteLitros += bonusLitros;
      valvulaFechada = false;
      digitalWrite(relayPin, LOW);
      digitalWrite(ledValvePin, LOW);

      Serial.print("Botão pressionado. +");
      Serial.print(bonusLitros);
      Serial.print("L liberados. Total bônus: ");
      Serial.println(bonusLiberadoContador);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Uso liberado");
      lcd.setCursor(0, 1);
      lcd.print("Bonus #");
      lcd.print(bonusLiberadoContador);
      delay(1000);
    }
  }

  if (estadoBotaoAtual == HIGH && botaoPressionadoAnterior == LOW) {
    botaoPressionadoAnterior = HIGH;
  }

  if (now - lastMeasure >= 1000) {
    lastMeasure = now;

    if (!valvulaFechada) {
      consumoLitros += ml_per_intervalo / 1000.0;

      lcd.setCursor(0, 0);
      lcd.print("Consumo: ");
      lcd.print(consumoLitros, 2);
      lcd.print("L    ");

      lcd.setCursor(0, 1);
      lcd.print("Fluxo: 10.0L/s ");

      Serial.print("Consumo atual: ");
      Serial.print(consumoLitros);
      Serial.println(" L");

      // Payload JSON para ThingsBoard
      String payload = "{\"litros\":" + String(consumoLitros, 2) + "}";
      client.publish("v1/devices/me/telemetry", payload.c_str());

      if (consumoLitros >= limiteLitros) {
        valvulaFechada = true;
        digitalWrite(relayPin, HIGH);
        digitalWrite(ledValvePin, HIGH);

        Serial.println("VÁLVULA FECHADA: Limite diário atingido.");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Limite atingido");
        lcd.setCursor(0, 1);
        lcd.print("Liberacoes: ");
        lcd.print(bonusLiberadoContador);
      }
    }
  }
}
