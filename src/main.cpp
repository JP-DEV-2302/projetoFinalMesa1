#include <Arduino.h>
#include <Lampada.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include "time.h"

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
// NTP
//==============================

const char* servidorNTP = "pool.ntp.org";
const long gmtOffset_sec = -3 * 3600;
const int daylightOffset_sec = 0;

//==============================
// TIMER
//==============================

int horaTimer = 0;
int minutoTimer = 0;
bool timerAtivo = false;
int acaoTimer = 0;
//==============================
// FUNÇÕES
//==============================

void tratarJsonLampada(const String& mensagem);
void tratarMensagemRecebida(const char* topico, const String& mensagem);
void configurarHorarioNTP();

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
    conectarWiFi();
    configurarHorarioNTP();
    configurarMQTT();
    registrarCallbackMensagem(tratarMensagemRecebida);
    conectarMQTT();
}

//==============================
// LOOP
//==============================

//==============================
// LOOP
//==============================

void loop()
{
    // Mantém Wi-Fi e MQTT conectados
    garantirWiFiConectado();
    garantirMQTTConectado();
    loopMQTT();

    // Atualiza os objetos Lampada
    interruptor1.update();
    interruptor2.update();
    interruptor3.update();
    interruptor4.update();

    // Executa a lógica abaixo apenas a cada 1 segundo
    static unsigned long ultimoDebug = 0;

    if (millis() - ultimoDebug >= 1000)
    {
        ultimoDebug = millis();

        struct tm timeinfo;

        // Obtém horário atual do NTP
        if (getLocalTime(&timeinfo))
        {
            int horaAtual    = timeinfo.tm_hour;
            int minutoAtual  = timeinfo.tm_min;
            int segundoAtual = timeinfo.tm_sec;

            // Exibe horário atual
            debugInfo(
                "Horario atual: " +
                String(horaAtual) + ":" +
                String(minutoAtual) + ":" +
                String(segundoAtual)
            );

            // Verifica se existe um timer ativo
            if (timerAtivo)
            {
                debugInfo(
                    "Timer ativo → " +
                    String(horaTimer) + ":" +
                    String(minutoTimer)
                );

                // Converte os horários para minutos
                int horarioAtualMinutos = (horaAtual * 60) + minutoAtual;
                int horarioTimerMinutos = (horaTimer * 60) + minutoTimer;

                // Executa quando chegar no horário programado
                if (horarioAtualMinutos >= horarioTimerMinutos)
                {
                    debugInfo("======================");
                    debugInfo("TIMER EXECUTADO");
                    debugInfo("======================");

                    debugInfo("Acao: " + String(acaoTimer));

                    // acao = 1 → ligar
                    if (acaoTimer == 1)
                    {
                        interruptor1.acender();
                        interruptor2.acender();
                        interruptor3.acender();
                        interruptor4.acender();

                        debugInfo("Interruptores ligados pelo timer");
                    }
                    // acao = 0 → desligar
                    else
                    {
                        interruptor1.apagar();
                        interruptor2.apagar();
                        interruptor3.apagar();
                        interruptor4.apagar();

                        debugInfo("Interruptores desligados pelo timer");
                    }

                    // Desativa o timer após executar
                    timerAtivo = false;
                }
            }
        }
        else
        {
            debugErro("Falha ao obter horario NTP");
        }
    }
}


//==============================
// NTP
//==============================

void configurarHorarioNTP()
{
    debugInfo("========================");
    debugInfo("Configurando NTP");
    debugInfo("========================");

    configTime(gmtOffset_sec, daylightOffset_sec, servidorNTP);

    struct tm timeinfo;

    if (!getLocalTime(&timeinfo))
    {
        debugErro("Falha ao obter horario NTP");
        return;
    }

    char horario[30];
    strftime(horario, sizeof(horario), "%H:%M:%S", &timeinfo);
    debugInfo("Horario sincronizado: " + String(horario));
}

//==============================
// CALLBACK MQTT
//==============================

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

    debugErro("Topico nao tratado");
}

//==============================
// JSON
//==============================

void tratarJsonLampada(const String& mensagem)
{
    JsonDocument doc;

    DeserializationError erro = deserializeJson(doc, mensagem);

    if (erro)
    {
        debugErro("Falha JSON: " + String(erro.c_str()));
        return;
    }

    debugInfo("JSON deserializado com sucesso");

    //==========================
    // SALA 09
    //==========================

    if (doc["lampadaSala09"].is<JsonObject>())
    {
        JsonObject sala09 = doc["lampadaSala09"].as<JsonObject>();

        debugInfo("Valor bruto interruptor1: " + String(sala09["interruptor1"].as<int>()));
        debugInfo("Valor bruto interruptor2: " + String(sala09["interruptor2"].as<int>()));

        bool estadoInterruptor1 = sala09["interruptor1"].as<int>() == 1;
        bool estadoInterruptor2 = sala09["interruptor2"].as<int>() == 1;

        estadoInterruptor1 ? interruptor1.acender() : interruptor1.apagar();
        estadoInterruptor2 ? interruptor2.acender() : interruptor2.apagar();

        debugInfo("interruptor1: " + String(estadoInterruptor1 ? "LIGADO" : "DESLIGADO"));
        debugInfo("interruptor2: " + String(estadoInterruptor2 ? "LIGADO" : "DESLIGADO"));

        if (sala09["timer"].is<JsonObject>())
        {
            JsonObject timer = sala09["timer"].as<JsonObject>();

            horaTimer   = timer["hora"] | 0;
            minutoTimer = timer["minuto"] | 0;
            timerAtivo  = timer["estado"].as<int>() == 1;
            acaoTimer = timer["acao"] | 0;

            debugInfo("======================");
            debugInfo("TIMER RECEBIDO");
            debugInfo("======================");
            debugInfo("Hora: " + String(horaTimer));
            debugInfo("Minuto: " + String(minutoTimer));
            debugInfo("Acao: " + String(acaoTimer));
        }
    }

    //==========================
    // SALA 10
    //==========================

    if (doc["lampadaSala10"].is<JsonObject>())
    {
        JsonObject sala10 = doc["lampadaSala10"].as<JsonObject>();

        debugInfo("Valor bruto interruptor3: " + String(sala10["interruptor3"].as<int>()));
        debugInfo("Valor bruto interruptor4: " + String(sala10["interruptor4"].as<int>()));

        bool estadoInterruptor3 = sala10["interruptor3"].as<int>() == 1;
        bool estadoInterruptor4 = sala10["interruptor4"].as<int>() == 1;

        estadoInterruptor3 ? interruptor3.acender() : interruptor3.apagar();
        estadoInterruptor4 ? interruptor4.acender() : interruptor4.apagar();

        debugInfo("interruptor3: " + String(estadoInterruptor3 ? "LIGADO" : "DESLIGADO"));
        debugInfo("interruptor4: " + String(estadoInterruptor4 ? "LIGADO" : "DESLIGADO"));

        if (sala10["timer"].is<JsonObject>())
        {
            JsonObject timer = sala10["timer"].as<JsonObject>();

          
            horaTimer   = timer["hora"] | 0;
            minutoTimer = timer["minuto"] | 0;
            timerAtivo  = timer["estado"].as<int>() == 1;
            acaoTimer = timer["acao"] | 0;

            debugInfo("======================");
            debugInfo("TIMER RECEBIDO");
            debugInfo("======================");
            debugInfo("Hora: " + String(horaTimer));
            debugInfo("Minuto: " + String(minutoTimer));
            debugInfo("Estado: " + String(timerAtivo));
        }
    }
}