# 💧 Sistema de Monitoramento de Consumo de Água com Limite Diário (ONU)

Este projeto simula um sistema inteligente de controle e monitoramento do consumo de água com base no limite sugerido pela ONU (110L por pessoa/dia). Desenvolvido com ESP32, LCD I2C e comunicação MQTT com integração à plataforma **ThingsBoard Cloud** para visualização remota dos dados.

---

## 🚀 Funcionalidades

- ✅ Monitoramento contínuo do consumo simulado de água
- 🚫 Válvula (simulada) é fechada ao atingir o limite de consumo
- 🔘 Botão libera bônus de +20L por vez
- 📟 Display LCD 16x2 mostra consumo e status em tempo real
- 📤 Dados enviados via MQTT para **ThingsBoard Cloud**
- 💡 LED indica se a válvula está fechada
- 📊 Painel de monitoramento remoto e gráfico

---

## 📦 Componentes Simulados

- **ESP32 DevKit V1**
- **LCD 16x2 I2C**
- **Módulo Relé**
- **LED Vermelho** (simula a válvula solenoide fechada)
- **Pushbutton** (liberação de bônus)
- **Broker MQTT (ThingsBoard Cloud)**

---

## 🌐 Comunicação MQTT

- **Broker**: `mqtt.thingsboard.cloud`
- **Porta**: `1883`
- **Tópico**: `v1/devices/me/telemetry`
- **Usuário (token do device)**: `JUAcqz8Sp52UcNl7FlGA`
- **Senha**: *(deixe em branco)*

###  🖥 Exibição no LCD
Linha 1: Consumo: XX.XXL

Linha 2: Fluxo: 10.0L/s (simulado)

Quando o limite é atingido:
Exibe: Limite atingido

Abaixo: Liberações: N

Quando o botão é pressionado:
Exibe: Uso liberado

Abaixo: Bonus #N

### 🛠 Instruções para Rodar no Wokwi
Acesse: https://wokwi.com

Importe os arquivos diagram.json e sketch.ino

Certifique-se de que:

O LCD está ligado ao VIN

Pinos SDA/SCL conectados ao D21 e D22

Rode a simulação — os dados começarão a aparecer no ThingsBoard

### 📊 Painel no ThingsBoard
Crie um Device no ThingsBoard Cloud

Copie o Token fornecido (ex: JUAcqz8Sp52UcNl7FlGA)

Vá até Latest Telemetry para ver os dados chegando

(Opcional) Crie um Dashboard com gráficos e indicadores para visualização.

