// ----- Monitoramento do Consumo de Água: Implementação de Sistema de Monitoramento com ESP32 e MQTT -----
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
const float litros_por_segundo = 10.0;  // Simula fluxo de 10L/s

unsigned long lastButtonPress = 0;
const unsigned long debounceDelay = 100;
bool botaoPressionadoAnterior = HIGH;

WiFiClientSecure espClient;
PubSubClient client(espClient);

unsigned long lastWifiCheck = 0;
const unsigned long wifiCheckInterval = 5000;  // 5 segundos

// -------- Publica Status --------
void publicarStatus(const String& status) {
  client.publish("device/esp32/status", status.c_str());
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

// -------- Comando MQTT  --------

  if (String(topic) == "device/esp32/command") {
    if (mensagem == "abrir") {
      liberarAgua();
    } else if (mensagem == "fechar") {
      fecharValvula();
    }
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
  }
}

// -------- Função fechar válvula --------
void fecharValvula() {
  if (!valvulaFechada) {
    valvulaFechada = true;
    digitalWrite(relayPin, HIGH);
    digitalWrite(ledValvePin, HIGH);

    Serial.println("Válvula fechada");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Valvula fechada");
    lcd.setCursor(0, 1);
    lcd.print("por comando");

    publicarStatus("Válvula fechada");
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

  conectarWiFi();

  espClient.setInsecure();  // Apenas para testes
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  conectarMQTT();

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

// -------- Conectar WiFi --------
void conectarWiFi() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Conect WiFi...");
  publicarStatus("Conectando Wi-Fi...");

  WiFi.begin(ssid, password);

  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 15000) {  // 15s timeout
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi conectado!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi OK");
    publicarStatus("Wi-Fi conectado!");
  } else {
    Serial.println("\nFalha ao conectar Wi-Fi");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi falhou");
    publicarStatus("Wi-Fi falhou");
  }
}

// -------- Conectar MQTT --------
void conectarMQTT() {
  if (WiFi.status() != WL_CONNECTED) return;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Conect MQTT...");
  publicarStatus("Conectando MQTT...");

  while (!client.connected()) {
    Serial.print("Tentando conectar MQTT...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("Conectado!");
      client.subscribe("device/esp32/command");

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
  unsigned long now = millis();

  // Checa WiFi periodicamente
  if (now - lastWifiCheck > wifiCheckInterval) {
    lastWifiCheck = now;
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Wi-Fi desconectado. Reconectando...");
      conectarWiFi();
      delay(1000);
    }
    if (!client.connected()) {
      Serial.println("MQTT desconectado. Reconectando...");
      conectarMQTT();
    }
  }

  client.loop();

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
  static unsigned long lastMeasure = 0;
  if (now - lastMeasure >= 1000) {
    lastMeasure = now;

    if (!valvulaFechada) {
      consumoLitros += litros_por_segundo;

      lcd.setCursor(0, 0);
      lcd.print("Consumo: ");
      lcd.print(consumoLitros, 2);
      lcd.print("L   ");

      lcd.setCursor(0, 1);
      lcd.print("Fluxo: 10.0L/s");

      String consumoStr = "{\"litros\":" + String(consumoLitros, 2) + "}";
      Serial.println("Telemetria: " + consumoStr);

      client.publish("device/esp32/telemetry", consumoStr.c_str());

      if (consumoLitros >= limiteLitros) {
        fecharValvula();

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
// -------- ALLAN PRADELLA FRUSHIO --------
