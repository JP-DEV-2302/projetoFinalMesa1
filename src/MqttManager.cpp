//! MqttManager.cpp
#include "MqttManager.h"
#include <Arduino.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "WiFiManager.h"
#include "secrets.h"

#include "DebugManager.h"

//==============INSTANCIAS===============
WiFiClient wifiClient;
WiFiClientSecure wifiClientSecure;
PubSubClient mqttClient;

CallbackMensagemMQTT callbackDaAplicacao = nullptr;

void registrarCallbackMensagem(CallbackMensagemMQTT callback)
{
    callbackDaAplicacao = callback;

    if (callbackDaAplicacao != nullptr)
    {
        debugInfo("Callback da aplicação registrada com sucesso.");
    }
    else
    {
        debugErro("Callback da aplicação não foi registrada.");
    }
}

const char *obterTopicoPublicacao(int indiceTopico)
{
    if (indiceTopico < 0 || indiceTopico >= TOTAL_TOPICOS_PUBLICAR)
    {
        debugErro("índice inválido para tópico de publicação: " + String(indiceTopico));
        return "";
    }

    return TOPICOS_PUBLICAR[indiceTopico];
}

const char *obterTopicoRecebimento(int indiceTopico)
{
    if (indiceTopico < 0 || indiceTopico >= TOTAL_TOPICOS_RECEBER)
    {
        debugErro("índice inválido para tópico de publicação: " + String(indiceTopico));
        return "";
    }

    return TOPICOS_RECEBER[indiceTopico];
}

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
    debugInfo("Tópico" + String(topico));
    debugInfo("Mensagem" + mensagem);

    if (callbackDaAplicacao != nullptr)
    {
        callbackDaAplicacao(topico, mensagem);
    }
    else
    {
        debugErro("Mensagem recebida,mas nenhum callback da aplicação foi registrada");
    }
}

void configurarMQTT()
{
    debugInfo("===============================");
    debugInfo("Configurando MQTT...");
    debugInfo("===============================");

    if (USAR_AWS_IOT)
    {
      wifiClientSecure.setCACert(AWS_CERT_CA);
wifiClientSecure.setCertificate(AWS_CERT_CRT);
wifiClientSecure.setPrivateKey(AWS_CERT_PRIVATE);

wifiClientSecure.setTimeout(12);

mqttClient.setClient(wifiClientSecure);
mqttClient.setServer(AWS_IOT_ENDPOINT, AWS_IOT_PORT);

        debugInfo("Endpoint AWS IoT : " + String(AWS_IOT_ENDPOINT));
        debugInfo("Porta AWS IoT : " + String(AWS_IOT_PORT));
    }
   else if (MQTT_TLS)
{
    debugInfo("Modo Selecionado: MQTT com TLS.");

    // Para testes usando HiveMQ Cloud
    wifiClientSecure.setInsecure();

    mqttClient.setClient(wifiClientSecure);
    mqttClient.setServer(MQTT_BROKER, MQTT_PORTA);

    debugInfo("Broker MQTT: " + String(MQTT_BROKER));
    debugInfo("Porta MQTT: " + String(MQTT_PORTA));
}
}

void conectarMQTT()
{
    if (!wifiEstaConectado())
    {
        debugErro("MQTT não pode conectar porque o WiFi está desconectado.");
        return;
    }

    debugInfo("=========================");
    debugInfo("Iniciando conexão MQTT...");
    debugInfo("=========================");

    int tentativasMQTT = 0;
    const int maxTentativasMQTT = 5;

    while (!mqttClient.connected() && tentativasMQTT < maxTentativasMQTT)
    {

        debugInfo("Tentando conectar ao broker MQTT. Tentativa:" + String(tentativasMQTT));

        bool conectado = false;

        if (USAR_AWS_IOT)
        {
           conectado = mqttClient.connect(AWS_IOT_CLIENT_ID);
        }
        else
        {
            if (strlen(MQTT_USUARIO) > 0)
            {
                debugInfo("Conectando MQTT com usuário e senha");

                conectado = mqttClient.connect(MQTT_CLIENT_ID, MQTT_USUARIO, MQTT_SENHA);

                debugInfo(String(conectado));
            }
            else // Sem usuário.
            {
                debugInfo("Conectando MQTT sem usuário e senha.");

                conectado = mqttClient.connect(MQTT_CLIENT_ID);
            }
        }
        if (conectado)
        {
            debugInfo("MQTT conectado com sucesso.");

            int totalTopicos = obterTotalTopicosRecebimento();

            debugInfo("Total de tópicos para inscrição: " + String(totalTopicos));

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
                    debugErro("Falha  ao se inscrever no tópico: " + String(topico));
                }
            }
            // TODO: publicar uma mensagem em um tópico informado em MQTT
        }
        else
        {
            debugErro("Falha ao conectar no MQTT. Código de erro:" + String(mqttClient.state()));

            tentativasMQTT++;
            delay(2000);
        }
    } // fim do while
    if (!mqttClient.connected())
    {
        debugErro("Não foi possivel conectar ao broker MQTT após " + String(maxTentativasMQTT) + "tentativas");
    }
}

void garantirMQTTConectado()
{
    if (!wifiEstaConectado())
    {
        debugErro("MQTT não será reconectado porque o WiFi está desconectado");
        return;
    }

    if (!mqttClient.connected())
    {
        debugErro("MQTT desconectado. Tentando reconectar...");
        conectarMQTT();
    }
}

void loopMQTT()
{
    mqttClient.loop();
}

void publicarMensagem(const char *topico, const char *mensagem)
{
    if (!mqttClient.connected())
    {
        debugErro("Não foi possivel publicar. MQTT desconectado");
        return;
    }

    bool publicado = mqttClient.publish(topico, mensagem);
    if (publicado)
    {
        debugInfo("Mensagem publicada via MQTT.");
        debugInfo("Topico: " + String(topico));
        debugInfo("Mensagem: " + String(mensagem));
    }
    else
    {
        debugErro("Falha ao publicar mensagem no tópico: " + String(topico));
    }
}

void publicarMensagemNoTopico(int indiceTopico, const char *mensagem)
{
    const char *topico = obterTopicoPublicacao(indiceTopico);

    if (strlen(topico) == 0)
    {
        debugErro("Não foi possivel publicar. Índice de tópico inválido : " + String(indiceTopico));
        return;
    }
    publicarMensagem(topico, mensagem);
}

bool mqttEstaConectado()
{
    return mqttClient.connected();
}

int obterTotalTopicosRecebimento()
{
    return TOTAL_TOPICOS_RECEBER;
}
