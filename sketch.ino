// -------- Config MQTT HiveMQ Cloud --------
//Monitoramento do Consumo de Água: 
//Implementação de Sistema de Monitoramento com 
//NodeMCU e MQTT.

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>

// -------- Config WiFi --------
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// -------- Config MQTT HiveMQ Cloud --------
const char* mqtt_server = "2036808a441b471f9b917cab5abc5d08.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_user = "user_mqtt";
const char* mqtt_password = "Ab123456";

// -------- Pinos --------
const int buttonPin = 2;
const int relayPin = 4;
const int ledValvePin = 5;

// -------- Variáveis --------
bool valvulaFechada = false;
float consumoLitros = 0;
float limiteLitros = 110.0;
float bonusLitros = 20.0;
int bonusLiberadoContador = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);
const float ml_per_intervalo = 10000.0;  // Simula 10L/s

// Botão físico
unsigned long lastButtonPress = 0;
const unsigned long debounceDelay = 300;
bool botaoPressionadoAnterior = HIGH;

// MQTT
WiFiClientSecure espClient;
PubSubClient client(espClient);

// -------- Publica Status --------
void publicarStatus(String status) {
  client.publish("valvula/status", status.c_str());
  Serial.println("Status: " + status);
}

// -------- Callback MQTT --------
void callback(char* topic, byte* payload, unsigned int length) {
  String mensagem;
  for (unsigned int i = 0; i < length; i++) {
    mensagem += (char)payload[i];
  }
  Serial.print("Mensagem recebida: ");
  Serial.println(mensagem);

  if (String(topic) == "valvula/control" && mensagem == "abrir") {
    liberarAgua();
  }
}

// -------- Função liberar água --------
void liberarAgua() {
  if (valvulaFechada) {
    bonusLiberadoContador++;
    limiteLitros += bonusLitros;
    valvulaFechada = false;
    digitalWrite(relayPin, LOW);
    digitalWrite(ledValvePin, LOW);

    Serial.println("Válvula aberta (MQTT ou botão)");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Uso liberado");
    lcd.setCursor(0, 1);
    lcd.print("Liberacao #");
    lcd.print(bonusLiberadoContador);

    publicarStatus("Uso liberado, Liberacao #" + String(bonusLiberadoContador));

    delay(1000);
  }
}

// -------- Setup --------
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
  publicarStatus("Iniciando...");

  WiFi.begin(ssid, password);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Conect WiFi...");
  publicarStatus("Conectando Wi-Fi...");

  Serial.print("Conectando Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado!");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi OK");
  publicarStatus("Wi-Fi conectado!");

  espClient.setInsecure();  

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  reconnect();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi + MQTT OK");
  publicarStatus("Wi-Fi + MQTT OK");

  delay(1500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Consumo: 0.00L");
  lcd.setCursor(0, 1);
  lcd.print("Fluxo: 10.0L/s");
}

// -------- Reconectar MQTT --------
void reconnect() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Conect MQTT...");
  publicarStatus("Conectando MQTT...");

  while (!client.connected()) {
    Serial.print("Tentando conectar MQTT...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("Conectado!");
      client.subscribe("valvula/control");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("MQTT OK");
      publicarStatus("MQTT conectado!");
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5s");
      delay(5000);
    }
  }
}

// -------- Loop principal --------
void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  unsigned long now = millis();
  static unsigned long lastMeasure = 0;

  // Botão físico
  bool estadoBotaoAtual = digitalRead(buttonPin);
  if (estadoBotaoAtual == LOW && botaoPressionadoAnterior == HIGH && (now - lastButtonPress > debounceDelay)) {
    lastButtonPress = now;
    botaoPressionadoAnterior = LOW;
    liberarAgua();
  }
  if (estadoBotaoAtual == HIGH && botaoPressionadoAnterior == LOW) {
    botaoPressionadoAnterior = HIGH;
  }

  // Consumo
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

      String consumoStr = "Consumo atual: " + String(consumoLitros, 2) + " L";
      Serial.println(consumoStr);

      client.publish("valvula/telemetria", consumoStr.c_str());

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

        publicarStatus("Válvula fechada - Limite atingido");
      }
    }
  }
}
