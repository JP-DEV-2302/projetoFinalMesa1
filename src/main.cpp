#include <Arduino.h>
#include <Lampada.h>
#include <ArduinoJson.h>

#include "WiFiManager.h"
#include "MqttManager.h"
#include "DebugManager.h"

const int pinoInterruptor1 = 18;
const int pinoInterruptor2 = 6;
const int pinoInterruptor3 = 7;
const int pinoInterruptor4 = 8;

Lampada interruptor1(pinoInterruptor1);
Lampada interruptor2(pinoInterruptor2);
Lampada interruptor3(pinoInterruptor3);
Lampada interruptor4(pinoInterruptor4);

void tratarJsonLampada(const String& mensagem);
void tratarMensagemRecebida(const char* topico, const String& mensagem);

const char TOPICO_COMANDO[] = "senai134/esp32/comando";

void setup()
{
    Serial.begin(115200);
    delay(3000);
    Serial.println("Iniciando...");

    pinMode(pinoInterruptor1, OUTPUT);
    pinMode(pinoInterruptor2, OUTPUT);
    pinMode(pinoInterruptor3, OUTPUT);
    pinMode(pinoInterruptor4, OUTPUT);

    configurarDebug();
    conectarWiFi();
    configurarMQTT();
    registrarCallbackMensagem(tratarMensagemRecebida);
    conectarMQTT();
}

void loop()
{
    garantirWiFiConectado();
    garantirMQTTConectado();
    loopMQTT();

    interruptor1.update();
    interruptor2.update();
    interruptor3.update();
    interruptor4.update();
}

void tratarMensagemRecebida(const char* topico, const String& mensagem)
{
    debugInfo("==============================");
    debugInfo("Mensagem recebida na aplicacao");
    debugInfo("==============================");

    if (topico == nullptr)
    {
        debugErro("Topico MQTT invalido");
        return;
    }

    debugInfo("Topico: " + String(topico));
    debugInfo("Mensagem: " + mensagem);

    if (strcmp(topico, TOPICO_COMANDO) == 0)
    {
        tratarJsonLampada(mensagem);
        return;
    }

    debugErro("Topico nao tratado: " + String(topico));
}

void tratarJsonLampada(const String& mensagem)
{
    // Passo 1 — Desserializa o JSON recebido
    JsonDocument doc;
    DeserializationError erro = deserializeJson(doc, mensagem);

    if (erro)
    {
        debugErro("Falha ao interpretar JSON: " + String(erro.c_str()));
        debugErro("Payload recebido: " + mensagem);
        return;
    }

    debugInfo("JSON deserializado com sucesso.");

    // Passo 2 — Extrai os valores dos interruptores das duas salas
    bool estadoInterruptor1 = false;
    bool estadoInterruptor2 = false;
    bool estadoInterruptor3 = false;
    bool estadoInterruptor4 = false;

    if (doc["lampadaSala09"].is<JsonObject>())
    {
        JsonObject sala09 = doc["lampadaSala09"].as<JsonObject>();
        estadoInterruptor1 = sala09["interruptor1"].as<bool>();
        estadoInterruptor2 = sala09["interruptor2"].as<bool>();
    }

    if (doc["lampadaSala10"].is<JsonObject>())
    {
        JsonObject sala10 = doc["lampadaSala10"].as<JsonObject>();
        estadoInterruptor3 = sala10["interruptor3"].as<bool>();
        estadoInterruptor4 = sala10["interruptor4"].as<bool>();
    }

    // Passo 3 — Switch case para tratar cada interruptor
    for (int i = 1; i <= 4; i++)
    {
        switch (i)
        {
            case 1:
                if (estadoInterruptor1)
                {
                    interruptor1.acender();
                    debugInfo("interruptor1: LIGADO");
                }
                else
                {
                    interruptor1.apagar();
                    debugInfo("interruptor1: DESLIGADO");
                }
                break;

            case 2:
                if (estadoInterruptor2)
                {
                    interruptor2.acender();
                    debugInfo("interruptor2: LIGADO");
                }
                else
                {
                    interruptor2.apagar();
                    debugInfo("interruptor2: DESLIGADO");
                }
                break;

            case 3:
                if (estadoInterruptor3)
                {
                    interruptor3.acender();
                    debugInfo("interruptor3: LIGADO");
                }
                else
                {
                    interruptor3.apagar();
                    debugInfo("interruptor3: DESLIGADO");
                }
                break;

            case 4:
                if (estadoInterruptor4)
                {
                    interruptor4.acender();
                    debugInfo("interruptor4: LIGADO");
                }
                else
                {
                    interruptor4.apagar();
                    debugInfo("interruptor4: DESLIGADO");
                }
                break;
        }
    }
}