# 💧 Sistema de Monitoramento de Consumo de Água com Limite Diário (ONU)

Este projeto simula um sistema inteligente de controle e monitoramento do consumo de água, com base nas recomendações da ONU para consumo diário de até 110 litros por pessoa. O sistema interrompe o fornecimento de água ao atingir esse limite e permite a liberação de bônus manuais de uso por meio de um botão físico.

---

## 📌 Funcionalidades

- ✅ Monitoramento do consumo de água em tempo real (simulado)
- ✅ Visualização do consumo e fluxo atual em LCD 16x2
- ✅ Corte automático do fornecimento ao atingir o limite de 110L
- ✅ Liberação de bônus de +20L por botão físico
- ✅ Indicação visual com LED quando a válvula estiver fechada
- ✅ Comunicação MQTT com broker seguro HiveMQ Cloud (TLS)

---

## 🧠 Tecnologias Utilizadas

- ESP32 DevKit v1
- LCD I2C 16x2
- Módulo Relé (simulando válvula solenoide)
- LED (indicador de válvula fechada)
- Push Button (liberação de bônus)
- Protocolo MQTT com autenticação TLS
- HiveMQ Cloud Serverless MQTT Broker
- Simulação via [Wokwi](https://wokwi.com/)

---

## ⚙️ Configuração

### 📁 Estrutura do projeto



### 🔧 Parâmetros de simulação

- Consumo simulado: `1 litro por segundo`
- Limite diário padrão: `110 litros`
- Bônus por botão: `20 litros`
- Comunicação via: `broker MQTT HiveMQ Cloud`
- Porta: `8883` (TLS)

---

## 📡 Comunicação MQTT

- **Broker**: `2036808a441b471f9b917cab5abc5d08.s1.eu.hivemq.cloud`
- **Porta**: `8883`
- **Username**: `allan_user`
- **Password**: `Senha123`
- **Tópico**: `casa/agua`

As mensagens são publicadas automaticamente com atualizações de consumo e alertas de válvula.

---

## 🖥️ LCD 16x2 (Display)

- Linha 1: `Consumo: 45.00L` ou `Limite atingido`
- Linha 2: `Fluxo: 1.00 L/s` ou `Liberações: X`

---

## 🔘 Botão

- Ao pressionar o botão **quando a válvula estiver fechada**, libera-se mais **20L** de consumo.
- O número de liberações é contabilizado e exibido no LCD e no Serial.

---

## 🚨 LED Indicador

- **Apagado** = válvula aberta (fluxo de água liberado)
- **Aceso** = válvula fechada (limite diário atingido)

---

## 🚀 Simule agora

Você pode simular o projeto completo gratuitamente em:

👉 https://wokwi.com/projects/429592043413401601

---

## 📄 Licença

Este projeto é livre para fins educacionais e está sob a licença MIT.

---

## 👨‍💻 Autor

**Allan Pradella**  
Monitoramento do Consumo de Água	: Implementação de Sistema de Monitoramento com NodeMCU e MQTT. — Mackenzie  
ODS 6 — Água potável e saneamento

