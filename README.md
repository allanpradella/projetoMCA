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
- **Usuário (Token do dispositivo)**: `JUAcqz8Sp52UcNl7FlGA`  
- **Senha**: *(deixe em branco)*  

### 📤 Payload enviado ao ThingsBoard:

```json
{
  "litros": 135.50
}
```

---

## 🖥 Exibição no LCD

- **Linha 1**: `Consumo: XX.XXL`  
- **Linha 2**: `Fluxo: 10.0L/s` (simulado)

### Quando o limite é atingido:
- Exibe: `Limite atingido`
- Linha 2: `Liberações: N` (quantidade de bônus usados)

### Quando o botão é pressionado:
- Exibe: `Uso liberado`
- Linha 2: `Bonus #N`

---

## 🛠 Instruções para Rodar no Wokwi

1. Acesse [Wokwi](https://wokwi.com/projects/429592043413401601)
2. Importe os arquivos `diagram.json` e `sketch.ino` do repositório
3. Conecte o **LCD** ao **VIN**, **GND**, **D21 (SDA)** e **D22 (SCL)**
4. Rode a simulação  
5. O sistema simula automaticamente o consumo de água em tempo real
6. Os dados serão enviados para o ThingsBoard Cloud

---

## 📊 Dashboard no ThingsBoard

1. Crie um dispositivo no [ThingsBoard Cloud](https://thingsboard.cloud)
2. Copie o token gerado e substitua na variável `mqtt_username` no código
3. Vá na aba **Latest Telemetry** para ver os dados em tempo real
4. (Opcional) Crie um painel com gráficos e indicadores personalizados

---

## 🧠 Contribuições

Este projeto foi desenvolvido para fins educacionais e demonstração de conceitos de IoT, MQTT e automação com microcontroladores. Sugestões e melhorias são bem-vindas!

---

## 📄 Licença

Este projeto está licenciado sob a licença MIT. Sinta-se livre para usá-lo e modificá-lo.

---
