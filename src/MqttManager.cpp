//! MqttManager.cpp

#include "MqttManager.h"

#include <Arduino.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

#include "WiFiManager.h"
#include "secrets.h"
#include "DebugManager.h"

//==============================
// INSTÂNCIAS
//==============================

WiFiClient wifiClient;
WiFiClientSecure wifiClientSecure;
PubSubClient mqttClient;

CallbackMensagemMQTT callbackDaAplicacao = nullptr;

//==============================
// CALLBACK DA APLICAÇÃO
//==============================

void registrarCallbackMensagem(CallbackMensagemMQTT callback)
{
    callbackDaAplicacao = callback;

    if (callbackDaAplicacao != nullptr)
    {
        debugInfo("Callback da aplicação registrada com sucesso.");
    }
    else
    {
        debugErro("Falha ao registrar callback da aplicação.");
    }
}

//==============================
// TÓPICOS
//==============================

const char *obterTopicoPublicacao(int indiceTopico)
{
    if (indiceTopico < 0 || indiceTopico >= TOTAL_TOPICOS_PUBLICAR)
    {
        debugErro("Índice inválido para tópico de publicação.");
        return "";
    }

    return TOPICOS_PUBLICAR[indiceTopico];
}

const char *obterTopicoRecebimento(int indiceTopico)
{
    if (indiceTopico < 0 || indiceTopico >= TOTAL_TOPICOS_RECEBER)
    {
        debugErro("Índice inválido para tópico de recebimento.");
        return "";
    }

    return TOPICOS_RECEBER[indiceTopico];
}

//==============================
// CALLBACK INTERNO MQTT
//==============================

void callbackInternoMQTT(char *topico, byte *payload, unsigned int tamanho)
{
    String mensagem = "";

    for (unsigned int i = 0; i < tamanho; i++)
    {
        mensagem += (char)payload[i];
    }

    debugInfo("================================");
    debugInfo("Mensagem MQTT recebida");
    debugInfo("================================");
    debugInfo("Topico: " + String(topico));
    debugInfo("Mensagem: " + mensagem);

    if (callbackDaAplicacao != nullptr)
    {
        callbackDaAplicacao(topico, mensagem);
    }
    else
    {
        debugErro("Nenhum callback da aplicação registrado.");
    }
}

//==============================
// CONFIGURAR MQTT
//==============================

void configurarMQTT()
{
    debugInfo("===============================");
    debugInfo("Configurando MQTT...");
    debugInfo("===============================");

    // Define o tamanho do buffer MQTT
    mqttClient.setBufferSize(1024);

    if (USAR_AWS_IOT)
    {
        debugInfo("Modo selecionado: AWS IoT");

        wifiClientSecure.setCACert(AWS_CERT_CA);
        wifiClientSecure.setCertificate(AWS_CERT_CRT);
        wifiClientSecure.setPrivateKey(AWS_CERT_PRIVATE);

        wifiClientSecure.setTimeout(12);

        mqttClient.setClient(wifiClientSecure);

        mqttClient.setServer(AWS_IOT_ENDPOINT, AWS_IOT_PORT);

        // REGISTRA CALLBACK
        mqttClient.setCallback(callbackInternoMQTT);

        debugInfo("Endpoint AWS IoT: " + String(AWS_IOT_ENDPOINT));
        debugInfo("Porta AWS IoT: " + String(AWS_IOT_PORT));
    }
    else if (MQTT_TLS)
    {
        debugInfo("Modo selecionado: MQTT TLS");

        wifiClientSecure.setInsecure();

        mqttClient.setClient(wifiClientSecure);

        mqttClient.setServer(MQTT_BROKER, MQTT_PORTA);

        // REGISTRA CALLBACK
        mqttClient.setCallback(callbackInternoMQTT);

        debugInfo("Broker MQTT: " + String(MQTT_BROKER));
        debugInfo("Porta MQTT: " + String(MQTT_PORTA));
    }
    else
    {
        debugInfo("Modo selecionado: MQTT sem TLS");

        mqttClient.setClient(wifiClient);

        mqttClient.setServer(MQTT_BROKER, MQTT_PORTA);

        // REGISTRA CALLBACK
        mqttClient.setCallback(callbackInternoMQTT);

        debugInfo("Broker MQTT: " + String(MQTT_BROKER));
        debugInfo("Porta MQTT: " + String(MQTT_PORTA));
    }
}

//==============================
// CONECTAR MQTT
//==============================

void conectarMQTT()
{
    if (!wifiEstaConectado())
    {
        debugErro("WiFi desconectado. MQTT não pode conectar.");
        return;
    }

    debugInfo("=========================");
    debugInfo("Iniciando conexão MQTT...");
    debugInfo("=========================");

    int tentativasMQTT = 0;
    const int MAX_TENTATIVAS = 5;

    while (!mqttClient.connected() && tentativasMQTT < MAX_TENTATIVAS)
    {
        debugInfo("Tentativa MQTT: " + String(tentativasMQTT));

        bool conectado = false;

        if (USAR_AWS_IOT)
        {
            conectado = mqttClient.connect(AWS_IOT_CLIENT_ID);
        }
        else
        {
            if (strlen(MQTT_USUARIO) > 0)
            {
                conectado = mqttClient.connect(
                    MQTT_CLIENT_ID,
                    MQTT_USUARIO,
                    MQTT_SENHA);
            }
            else
            {
                conectado = mqttClient.connect(MQTT_CLIENT_ID);
            }
        }

        if (conectado)
        {
            debugInfo("MQTT conectado com sucesso.");

            int totalTopicos = obterTotalTopicosRecebimento();

            debugInfo("Total de tópicos: " + String(totalTopicos));

            for (int i = 0; i < totalTopicos; i++)
            {
                const char *topico = obterTopicoRecebimento(i);

                bool inscrito = mqttClient.subscribe(topico);

                if (inscrito)
                {
                    debugInfo("Inscrito no tópico: " + String(topico));
                }
                else
                {
                    debugErro("Falha ao inscrever no tópico: " + String(topico));
                }
            }
        }
        else
        {
            debugErro("Falha MQTT. Código: " + String(mqttClient.state()));

            tentativasMQTT++;

            delay(2000);
        }
    }

    if (!mqttClient.connected())
    {
        debugErro("Não foi possível conectar ao MQTT.");
    }
}

//==============================
// GARANTIR MQTT
//==============================

void garantirMQTTConectado()
{
    if (!wifiEstaConectado())
    {
        debugErro("WiFi desconectado.");
        return;
    }

    if (!mqttClient.connected())
    {
        debugErro("MQTT desconectado. Reconectando...");
        conectarMQTT();
    }
}

//==============================
// LOOP MQTT
//==============================

void loopMQTT()
{
    mqttClient.loop();
}

//==============================
// PUBLICAR
//==============================

void publicarMensagem(const char *topico, const char *mensagem)
{
    if (!mqttClient.connected())
    {
        debugErro("MQTT desconectado. Não foi possível publicar.");
        return;
    }

    bool publicado = mqttClient.publish(topico, mensagem);

    if (publicado)
    {
        debugInfo("Mensagem publicada.");
        debugInfo("Topico: " + String(topico));
        debugInfo("Mensagem: " + String(mensagem));
    }
    else
    {
        debugErro("Falha ao publicar mensagem.");
    }
}

void publicarMensagemNoTopico(int indiceTopico, const char *mensagem)
{
    const char *topico = obterTopicoPublicacao(indiceTopico);

    if (strlen(topico) == 0)
    {
        debugErro("Índice de tópico inválido.");
        return;
    }

    publicarMensagem(topico, mensagem);
}

//==============================
// STATUS
//==============================

bool mqttEstaConectado()
{
    return mqttClient.connected();
}

int obterTotalTopicosRecebimento()
{
    return TOTAL_TOPICOS_RECEBER;
}