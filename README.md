***

# 💡 Projeto Sala Inteligente - Módulo Lâmpada (ESP32)

> Solução modular de IoT para acionamento inteligente de iluminação, focado em automação, confiabilidade e eficiência energética para instituições de ensino.

---

## 📌 1. Sobre o Projeto
O **Módulo de Controle de Lâmpadas** foi desenvolvido como parte de um ecossistema inteligente de IoT para a Turing Serviços em T.I., visando modernizar ambientes educacionais e evitar o desperdício massivo de energia elétrica em salas e pátios vazios. 

O sistema integra o controle de iluminação à grade horária escolar, permitindo agendamentos dinâmicos e operação remota via dashboard, sem remover a autonomia local de uso por meio de interruptores e botões de parede, atuando como modo de contingência (fallback).

## 🚀 2. Principais Funcionalidades
- **Acionamento Remoto:** Controle independente das lâmpadas via comandos de nuvem e mensagens MQTT enviadas por um painel.
- **Sincronização NTP e Timer Dinâmico:** Conexão com servidores globais de horário (`pool.ntp.org`) para execução autônoma de agendamentos no ESP32, possibilitando que áreas externas desliguem a luz automaticamente após o período letivo.
- **Atualização OTA (Over-the-Air):** Mecanismo de reflash e atualização do firmware remotamente e sem fio, equipado com proteção de falhas e fallback de bootloader (`esp_ota_ops.h`).
- **Comunicação Segura:** Módulo MQTT totalmente não-bloqueante pronto para uso com ou sem TLS, suportando criptografia avançada e certificados nativos para **AWS IoT Core**.
- **Gerenciador de Debug:** Sistema configurável de logs e monitoramento serial. A verbosidade pode ser alterada via código ou forçando um pino físico (jumper) para modo verboso total em campo.

## 🛠️ 3. Hardware e Arquitetura
O processamento embarcado é gerenciado pelo ESP32, que se comunica com um sistema de relés e chaves táteis.

- **Microcontrolador:** Placa modelo `ESP32-S3-DevKitM-1`.
- **Atuadores (Isolamento Óptico):** Módulos Relé de Estado Sólido (`SSR_10DD`), dimensionados para acionar com segurança a carga das lâmpadas AC de 110V/220V recebendo sinais de 3,3V via GPIO.
- **Inputs:** Push buttons mapeados nos pinos `35` a `38` para o toggle físico de cada circuito.

## 📦 4. Dependências de Software (PlatformIO)
O gerenciamento do ambiente e bibliotecas é feito nativamente via PlatformIO (`platformio.ini`). O projeto requer as seguintes bibliotecas:

```ini
lib_deps = 
    [https://github.com/JP-DEV-2302/Biblioteca-Lampada.git](https://github.com/JP-DEV-2302/Biblioteca-Lampada.git)
    bblanchon/ArduinoJson@7.4.3
    knolleary/PubSubClient@^2.8
    professorthiago/AtualizadorOTA@^1.0.0
```

## ⚙️ 5. Configuração e Instalação

1. Clone este repositório no seu ambiente de desenvolvimento.
2. Abra o diretório na sua IDE suportada (recomendado: **Visual Studio Code** com extensão **PlatformIO IDE**).
3. **Credenciais Seguras:** Na raiz da pasta `include/`, você notará o arquivo `secrets.h`. Crie ou garanta que o respectivo `secrets.cpp` esteja na pasta `src/` (normalmente ignorado pelo `.gitignore`) com as suas informações:
   - SSID e Senha do seu Wi-Fi (`WIFI_SSID`, `WIFI_SENHA`).
   - Credenciais, portas e endpoints do seu broker MQTT.
   - Certificados CA e chaves privadas caso esteja utilizando AWS IoT.
4. Conecte o ESP32 ao computador via cabo de dados, selecione o ambiente `[env:sala09]` ou afins e clique em **Upload**. Os uploads futuros podem ser feitos pelas portas de rede (`espota`).

## 📡 6. Protocolo e Estrutura de Comunicação (JSON)
A troca de mensagens com o Broker MQTT ocorre preferencialmente via serialização de documentos JSON (implementada pela *ArduinoJson*).

**Exemplo de Payload de Comando / Agendamento recebido (Subscribe):**

```json
{
  "lampadaSala09": {
    "interruptor1": 1,
    "interruptor2": 0,
    "timer": {
      "hora": 23,
      "minuto": 0,
      "estado": 1,
      "acao": 0
    }
  }
}
```

**Exemplo de Evento Local enviado ao Broker pelo ESP32 (Publish):**
Sempre que um usuário aperta o interruptor físico na parede, o hardware avisa imediatamente o painel publicando:

```json
{
  "timestamp": "2026-05-27T10:15:30",
  "origem": "botao",
  "lampadaSala10": {
    "interruptor4": 1
  }
}
```

## 👥 7. Equipe de Desenvolvimento
Projeto acadêmico e de arquitetura de software elaborado no formato corporativo IoT na **Escola Senai Paulo Antônio Skaf**, São Paulo:

* **Eric Tsuruta**
* **João Pedro Maximiniano de Oliveira**
* **Manuella Huertas Bettini**
* **Marcos de Paula Moser**
* **Neyton Duarte dos Santos**
