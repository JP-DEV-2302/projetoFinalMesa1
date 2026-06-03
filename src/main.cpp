#include <Arduino.h>
#include <Lampada.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <Update.h>
#include "time.h"

#include "WiFiManager.h"
#include "MqttManager.h"
#include "DebugManager.h"

//==============================
// PINOS
//==============================

const int pinoInterruptor1 = 37;
const int pinoInterruptor2 = 38;
const int pinoInterruptor3 = 39;
const int pinoInterruptor4 = 40;

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
// BOTÃO MQTT
//==============================

const int pinoBotao  = 35;
const int pinoBotao2 = 36;
const int pinoBotao3 = 45;
const int pinoBotao4 = 48;

bool estadoBotaoAnterior  = HIGH;
bool estadoBotaoAnterior2 = HIGH;
bool estadoBotaoAnterior3 = HIGH;
bool estadoBotaoAnterior4 = HIGH;
bool estadoLampadaBotao   = false; // controla interruptor1
bool estadoLampadaBotao2  = false; // controla interruptor2
bool estadoLampadaBotao3  = false; // controla interruptor2
bool estadoLampadaBotao4  = false; // controla interruptor2

//==============================
// FUNÇÕES
//==============================

void tratarJsonLampada(const String& mensagem);
void tratarMensagemRecebida(const char* topico, const String& mensagem);
void configurarHorarioNTP();
String obterTimestamp();

//==============================
// TIMESTAMP
//==============================

String obterTimestamp()
{
    struct tm timeinfo;

    if (!getLocalTime(&timeinfo))
    {
        return "";
    }

    char buffer[25];

    strftime(
        buffer,
        sizeof(buffer),
        "%Y-%m-%dT%H:%M:%S",
        &timeinfo
    );

    return String(buffer);
}

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

    pinMode(pinoBotao,  INPUT_PULLUP);
    pinMode(pinoBotao2, INPUT_PULLUP);
    pinMode(pinoBotao3, INPUT_PULLUP);
    pinMode(pinoBotao4, INPUT_PULLUP);

// ArduinoOTA.setHostname("esp-sala09");
// ArduinoOTA.onStart([]()
// {
//     Serial.println("Iniciando OTA");
// });

// ArduinoOTA.onEnd([]()
// {
//     Serial.println("OTA concluido");
// });




  configurarDebug();

conectarWiFi();

ArduinoOTA.setHostname("lampadaSala10");

ArduinoOTA.onStart([]()
{
    Serial.println("Iniciando OTA");
});

ArduinoOTA.onEnd([]()
{
    Serial.println("OTA concluido");
});

ArduinoOTA.begin();
Serial.println("OTA INICIADO");
Serial.print("IP: ");
Serial.println(WiFi.localIP());


configurarHorarioNTP();

configurarMQTT();
registrarCallbackMensagem(tratarMensagemRecebida);
conectarMQTT();

}

//==============================
// LOOP
//==============================

void loop()
{
    ArduinoOTA.handle();

    // garantirWiFiConectado();
    // garantirMQTTConectado();
    loopMQTT();



    
    //==============================
    // BOTÃO 4 — Pino 48 → interruptor4
    //==============================

    bool estadoBotaoAtual4 = digitalRead(pinoBotao4);

    if (estadoBotaoAnterior4 == HIGH && estadoBotaoAtual4 == LOW)
    {
        estadoLampadaBotao4 = !estadoLampadaBotao4;

        JsonDocument doc;
        doc["timestamp"] = obterTimestamp();
        doc["origem"]    = "botao";
        doc["lampadaSala10"]["interruptor4"] = estadoLampadaBotao4 ? 1 : 0; // ✅ CORRIGIDO

        String mensagem;
        serializeJson(doc, mensagem);

        publicarMensagem(TOPICO_COMANDO, mensagem.c_str());

        debugInfo("======================");
        debugInfo("BOTAO 4 PRESSIONADO");
        debugInfo("======================");
        debugInfo("JSON ENVIADO:");
        debugInfo(mensagem);

        delay(250);
    }

    estadoBotaoAnterior4 = estadoBotaoAtual4;

    
    //==============================
    // BOTÃO 3 — Pino 45 → interruptor3
    //==============================

    bool estadoBotaoAtual3 = digitalRead(pinoBotao3);

    if (estadoBotaoAnterior3 == HIGH && estadoBotaoAtual3 == LOW)
    {
        estadoLampadaBotao3 = !estadoLampadaBotao3;

        JsonDocument doc;
        doc["timestamp"] = obterTimestamp();
        doc["origem"]    = "botao";
        doc["lampadaSala10"]["interruptor3"] = estadoLampadaBotao3 ? 1 : 0; // ✅ CORRIGIDO

        String mensagem;
        serializeJson(doc, mensagem);

        publicarMensagem(TOPICO_COMANDO, mensagem.c_str());

        debugInfo("======================");
        debugInfo("BOTAO 3 PRESSIONADO");
        debugInfo("======================");
        debugInfo("JSON ENVIADO:");
        debugInfo(mensagem);

        delay(250);
    }

    estadoBotaoAnterior3 = estadoBotaoAtual3;

    //==============================
    // BOTÃO 2 — Pino 36 → interruptor2
    //==============================

    bool estadoBotaoAtual2 = digitalRead(pinoBotao2);

    if (estadoBotaoAnterior2 == HIGH && estadoBotaoAtual2 == LOW)
    {
        estadoLampadaBotao2 = !estadoLampadaBotao2;

        JsonDocument doc;
        doc["timestamp"] = obterTimestamp();
        doc["origem"]    = "botao";
        doc["lampadaSala09"]["interruptor2"] = estadoLampadaBotao2 ? 1 : 0; // ✅ CORRIGIDO

        String mensagem;
        serializeJson(doc, mensagem);

        publicarMensagem(TOPICO_COMANDO, mensagem.c_str());

        debugInfo("======================");
        debugInfo("BOTAO 2 PRESSIONADO");
        debugInfo("======================");
        debugInfo("JSON ENVIADO:");
        debugInfo(mensagem);

        delay(250);
    }

    estadoBotaoAnterior2 = estadoBotaoAtual2;

    //==============================
    // BOTÃO 1 — Pino 35 → interruptor1
    //==============================

    bool estadoBotaoAtual = digitalRead(pinoBotao);

    if (estadoBotaoAnterior == HIGH && estadoBotaoAtual == LOW)
    {
        estadoLampadaBotao = !estadoLampadaBotao;

        JsonDocument doc;
        doc["timestamp"] = obterTimestamp();
        doc["origem"]    = "botao";
        doc["lampadaSala09"]["interruptor1"] = estadoLampadaBotao ? 1 : 0; // ✅ CORRIGIDO

        String mensagem;
        serializeJson(doc, mensagem);

        publicarMensagem(TOPICO_COMANDO, mensagem.c_str());

        debugInfo("======================");
        debugInfo("BOTAO PRESSIONADO");
        debugInfo("======================");
        debugInfo("JSON ENVIADO:");
        debugInfo(mensagem);

        delay(250);
    }

    estadoBotaoAnterior = estadoBotaoAtual;

    //==============================
    // UPDATE DAS LÂMPADAS
    //==============================

    interruptor1.update();
    interruptor2.update();
    interruptor3.update();
    interruptor4.update();

    //==============================
    // TIMER / DEBUG NTP
    //==============================

    static unsigned long ultimoDebug = 0;

    if (millis() - ultimoDebug >= 1000)
    {
        ultimoDebug = millis();

        struct tm timeinfo;

        if (getLocalTime(&timeinfo))
        {
            int horaAtual    = timeinfo.tm_hour;
            int minutoAtual  = timeinfo.tm_min;
            int segundoAtual = timeinfo.tm_sec;

            debugInfo(
                "Horario atual: " +
                String(horaAtual)    + ":" +
                String(minutoAtual)  + ":" +
                String(segundoAtual)
            );

            if (timerAtivo)
            {
                debugInfo(
                    "Timer ativo → " +
                    String(horaTimer) + ":" +
                    String(minutoTimer)
                );

                int horarioAtualMinutos  = (horaAtual   * 60) + minutoAtual;
                int horarioTimerMinutos  = (horaTimer   * 60) + minutoTimer;

                if (horarioAtualMinutos >= horarioTimerMinutos)
                {
                    debugInfo("======================");
                    debugInfo("TIMER EXECUTADO");
                    debugInfo("======================");
                    debugInfo("Acao: " + String(acaoTimer));

                    if (acaoTimer == 1)
                    {
                        interruptor1.acender();
                        interruptor2.acender();
                        interruptor3.acender();
                        interruptor4.acender();

                        estadoLampadaBotao  = true;
                        estadoLampadaBotao2 = true; // ✅ sincroniza botão 2 também
                         estadoLampadaBotao3 = true;
                        estadoLampadaBotao4 = true;

                        debugInfo("Interruptores ligados pelo timer");
                    }
                    else
                    {
                        interruptor1.apagar();
                        interruptor2.apagar();
                        interruptor3.apagar();
                        interruptor4.apagar();

                        estadoLampadaBotao  = false;
                        estadoLampadaBotao2 = false; // ✅ sincroniza botão 2 também
                        estadoLampadaBotao3 = false;
                        estadoLampadaBotao4 = false;

                        debugInfo("Interruptores desligados pelo timer");
                    }

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

    debugInfo("Topico: "   + String(topico));
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

     if (sala09["interruptor1"].is<int>())
{
    bool estadoInterruptor1 = sala09["interruptor1"].as<int>() == 1;

    estadoLampadaBotao = estadoInterruptor1;

    estadoInterruptor1
        ? interruptor1.acender()
        : interruptor1.apagar();

    debugInfo(
        "interruptor1: " +
        String(estadoInterruptor1 ? "LIGADO" : "DESLIGADO")
    );
}

if (sala09["interruptor2"].is<int>())
{
    bool estadoInterruptor2 = sala09["interruptor2"].as<int>() == 1;

    estadoLampadaBotao2 = estadoInterruptor2;

    estadoInterruptor2
        ? interruptor2.acender()
        : interruptor2.apagar();

    debugInfo(
        "interruptor2: " +
        String(estadoInterruptor2 ? "LIGADO" : "DESLIGADO")
    );
}

        // debugInfo("interruptor1: " + String(innterruptor1 ? "LIGADO" : "DESLIGADO"));
        // debugInfo("interruptor2: " + String(estadoInterruptor2 ? "LIGADO" : "DESLIGADO"));

        if (sala09["timer"].is<JsonObject>())
        {
            JsonObject timer = sala09["timer"].as<JsonObject>();

            horaTimer   = timer["hora"]   | 0;
            minutoTimer = timer["minuto"] | 0;
            timerAtivo  = timer["estado"].as<int>() == 1;
            acaoTimer   = timer["acao"]   | 0;

            debugInfo("======================");
            debugInfo("TIMER RECEBIDO");
            debugInfo("======================");
            debugInfo("Hora: "   + String(horaTimer));
            debugInfo("Minuto: " + String(minutoTimer));
            debugInfo("Acao: "   + String(acaoTimer));
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

      if (sala10["interruptor3"].is<int>())
{
    bool estadoInterruptor3 = sala10["interruptor3"].as<int>() == 1;

    estadoLampadaBotao3 = estadoInterruptor3;

    estadoInterruptor3
        ? interruptor3.acender()
        : interruptor3.apagar();

    debugInfo(
        "interruptor3: " +
        String(estadoInterruptor3 ? "LIGADO" : "DESLIGADO")
    );
}

if (sala10["interruptor4"].is<int>())
{
    bool estadoInterruptor4 = sala10["interruptor4"].as<int>() == 1;

    estadoLampadaBotao2 = estadoInterruptor4;

    estadoInterruptor4
        ? interruptor4.acender()
        : interruptor4.apagar();

    debugInfo(
        "interruptor4: " +
        String(estadoInterruptor4 ? "LIGADO" : "DESLIGADO")
    );
}

        // debugInfo("interruptor3: " + String(estadoInterruptor3 ? "LIGADO" : "DESLIGADO"));
        // debugInfo("interruptor4: " + String(estadoInterruptor4 ? "LIGADO" : "DESLIGADO"));

        if (sala10["timer"].is<JsonObject>())
        {
            JsonObject timer = sala10["timer"].as<JsonObject>();

            horaTimer   = timer["hora"]   | 0;
            minutoTimer = timer["minuto"] | 0;
            timerAtivo  = timer["estado"].as<int>() == 1;
            acaoTimer   = timer["acao"]   | 0;

            debugInfo("======================");
            debugInfo("TIMER RECEBIDO");
            debugInfo("======================");
            debugInfo("Hora: "   + String(horaTimer));
            debugInfo("Minuto: " + String(minutoTimer));
            debugInfo("Estado: " + String(timerAtivo));
        }
    }
}