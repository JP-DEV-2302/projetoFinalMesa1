#include <Arduino.h>
#include <Lampada.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <ezTime.h>

#include "WiFiManager.h"
#include "MqttManager.h"
#include "DebugManager.h"

//==============================
// PINOS
//==============================

const int pinoInterruptor1 = 18;
const int pinoInterruptor2 = 6;
const int pinoInterruptor3 = 7;
const int pinoInterruptor4 = 8;

//==============================
// OBJETOS
//==============================

Lampada interruptor1(pinoInterruptor1);
Lampada interruptor2(pinoInterruptor2);
Lampada interruptor3(pinoInterruptor3);
Lampada interruptor4(pinoInterruptor4);

//==============================
// MQTT
//==============================

const char TOPICO_COMANDO[] = "senai134/esp32/comando";

//==============================
// EZTIME
//==============================

Timezone tempoBrasil;

//==============================
// TIMER
//==============================

int horaTimer = 0;
int minutoTimer = 0;
bool timerAtivo = false;

//==============================
// FUNÇÕES
//==============================

void tratarJsonLampada(const String& mensagem);
void tratarMensagemRecebida(const char* topico, const String& mensagem);

//==============================
// SETUP
//==============================

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

    //==========================
    // WIFI
    //==========================

    conectarWiFi();

    //==========================
    // EZTIME
    //==========================

    waitForSync();

    tempoBrasil.setLocation("America/Sao_Paulo");

    debugInfo("======================");
    debugInfo("Horario sincronizado");
    debugInfo("======================");

    debugInfo(
        "Horario atual: " +
        tempoBrasil.dateTime()
    );

    //==========================
    // MQTT
    //==========================

    configurarMQTT();

    registrarCallbackMensagem(
        tratarMensagemRecebida
    );

    conectarMQTT();
}

//==============================
// LOOP
//==============================

void loop()
{
    garantirWiFiConectado();

    garantirMQTTConectado();

    loopMQTT();

    events();

    interruptor1.update();
    interruptor2.update();
    interruptor3.update();
    interruptor4.update();

    //==========================
    // HORARIO ATUAL
    //==========================

    int horaAtual =
        tempoBrasil.hour();

    int minutoAtual =
        tempoBrasil.minute();

    int segundoAtual =
        tempoBrasil.second();

    debugInfo(
        "Horario atual: " +
        String(horaAtual) + ":" +
        String(minutoAtual) + ":" +
        String(segundoAtual)
    );

    //==========================
    // TIMER
    //==========================

    if (timerAtivo)
    {
        debugInfo("Timer ativo");

        debugInfo(
            "Horario timer: " +
            String(horaTimer) + ":" +
            String(minutoTimer)
        );

        int horarioAtualMinutos =
            (horaAtual * 60) + minutoAtual;

        int horarioTimerMinutos =
            (horaTimer * 60) + minutoTimer;

        if (horarioAtualMinutos >= horarioTimerMinutos)
        {
            interruptor1.apagar();

            debugInfo("======================");
            debugInfo("TIMER EXECUTADO");
            debugInfo("======================");

            debugInfo(
                "Interruptor1 desligado"
            );

            timerAtivo = false;
        }
    }

    delay(1000);
}

//==============================
// CALLBACK MQTT
//==============================

void tratarMensagemRecebida(
    const char* topico,
    const String& mensagem)
{
    debugInfo("==============================");
    debugInfo("Mensagem recebida");
    debugInfo("==============================");

    if (topico == nullptr)
    {
        debugErro("Topico MQTT invalido");
        return;
    }

    debugInfo(
        "Topico: " +
        String(topico)
    );

    debugInfo(
        "Mensagem: " +
        mensagem
    );

    if (strcmp(topico, TOPICO_COMANDO) == 0)
    {
        tratarJsonLampada(mensagem);
        return;
    }

    debugErro("Topico nao tratado");
}

//==============================
// JSON
//==============================

void tratarJsonLampada(
    const String& mensagem)
{
    DynamicJsonDocument doc(1024);

    DeserializationError erro =
        deserializeJson(doc, mensagem);

    if (erro)
    {
        debugErro(
            "Falha JSON: " +
            String(erro.c_str())
        );

        return;
    }

    debugInfo(
        "JSON deserializado com sucesso"
    );

    //==========================
    // SALA 09
    //==========================

    if (doc["lampadaSala09"].is<JsonObject>())
    {
        JsonObject sala09 =
            doc["lampadaSala09"]
            .as<JsonObject>();

        bool estadoInterruptor1 =
            sala09["interruptor1"] | false;

        bool estadoInterruptor2 =
            sala09["interruptor2"] | false;

        if (estadoInterruptor1)
        {
            interruptor1.acender();

            debugInfo(
                "interruptor1: LIGADO"
            );
        }
        else
        {
            interruptor1.apagar();

            debugInfo(
                "interruptor1: DESLIGADO"
            );
        }

        if (estadoInterruptor2)
        {
            interruptor2.acender();

            debugInfo(
                "interruptor2: LIGADO"
            );
        }
        else
        {
            interruptor2.apagar();

            debugInfo(
                "interruptor2: DESLIGADO"
            );
        }
    }

    //==========================
    // SALA 10
    //==========================

    if (doc["lampadaSala10"].is<JsonObject>())
    {
        JsonObject sala10 =
            doc["lampadaSala10"]
            .as<JsonObject>();

        bool estadoInterruptor3 =
            sala10["interruptor3"] | false;

        bool estadoInterruptor4 =
            sala10["interruptor4"] | false;

        if (estadoInterruptor3)
        {
            interruptor3.acender();

            debugInfo(
                "interruptor3: LIGADO"
            );
        }
        else
        {
            interruptor3.apagar();

            debugInfo(
                "interruptor3: DESLIGADO"
            );
        }

        if (estadoInterruptor4)
        {
            interruptor4.acender();

            debugInfo(
                "interruptor4: LIGADO"
            );
        }
        else
        {
            interruptor4.apagar();

            debugInfo(
                "interruptor4: DESLIGADO"
            );
        }
    }

    //==========================
    // TIMER
    //==========================

    if (doc["timer"].is<JsonObject>())
    {
        JsonObject timer =
            doc["timer"]
            .as<JsonObject>();

        horaTimer =
            timer["hora"] | 0;

        minutoTimer =
            timer["minuto"] | 0;

        timerAtivo =
            timer["estado"] | false;

        debugInfo("======================");
        debugInfo("TIMER RECEBIDO");
        debugInfo("======================");

        debugInfo(
            "Hora: " +
            String(horaTimer)
        );

        debugInfo(
            "Minuto: " +
            String(minutoTimer)
        );

        debugInfo(
            "Estado: " +
            String(timerAtivo)
        );
    }
}