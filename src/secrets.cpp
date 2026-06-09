#include "secrets.h"
#include <Arduino.h>

const char *WIFI_SSID = "SALA 09";
const char *WIFI_SENHA = "info@134";

//=============================
// MQTT
//=============================

const bool MQTT_TLS = true;
const char *MQTT_BROKER = "7446153920f943aaafdb9d624f064dbb.s1.eu.hivemq.cloud";
const int MQTT_PORTA = 8883;

const char *MQTT_CLIENT_ID = "iotconsole-9bafb6d5-c297-4f58-bec1-ba5a49714d0c";

const char *MQTT_USUARIO = "CA0S6";
const char *MQTT_SENHA = "Senai@134";

const bool MQTT_USAR_TLS = false; // modo de cnexao sem segurança

const char MQTT_CERTIFICADO_CA[] PROGMEM = "";

//=============================
// AWS
//=============================

const bool USAR_AWS_IOT = true;

// Troque a linha fixa por isso:
#if SALA == 9
const char *AWS_IOT_CLIENT_ID = "luigi-sala09";
#elif SALA == 10
const char *AWS_IOT_CLIENT_ID = "luigi-sala10";
#else
const char *AWS_IOT_CLIENT_ID = "luigi-123";
#endif

const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

const char AWS_CERT_CRT[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUacscyH2YErhRy+tBPjMN6aG/9pcwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI2MDUyMDIwMDEw
OVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAK/SCNdT4SxoP7EENXEe
y91Fz94MsYwh5mtDWi0kIgbejJ3EwTCGtOOlwTL4zNEwOobC+FGB+VSfGyi05e8l
gWjIiVYu5LGjQPzel2t1PnEetNH3VVuNhYejS75yJRIFsf5QhXcMSGjvb8pON3od
rorCc9tcsHtzCTb2Bt82xjOh5+/8wabF5zJVdHEwmngShokONVVRZOraFm/vF60C
sonocSL8Yj4eLDDq7avd696OrowuWgoUtkpoQ6Tb+Bd/+068yGnHiYAjybXrcpGp
f5jj3vU5izuPEWpb3N7W+8Pc8Sdb3AQAw26Rf+vDbcXMxtprvjAc/BvTuPwedbPD
cAECAwEAAaNgMF4wHwYDVR0jBBgwFoAUxvd1JIanu4VaLedU1rXA/cwavnMwHQYD
VR0OBBYEFDP0VM/Dn7Ja8wQ/rsG8ZZcCFY0UMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQC28zw5nthIGDVtLgOqIqz+NdZo
13b0lRGaS+PnpxcaZ9u+6iMohcXueHeC68gVqhIjJAXm4F0eSqk8+Cu5cAbjx0f9
GJUKQo5tlmwPMoRtpVZJwvHCaFa6LK02Tp2oVvB0rwCUZUwjMXi69MJUSCtJ/j82
bpyN7mgIoQK6D5M8cw/IbQ9W1AqRd4b+6tRplKUIi4KuDh1DGxqN+25MfH7hhZOA
pMwCfbDtX+SHsffcnccCXif+KCZaH+dJKgCq8DpQ2SvoYWRui1x7dT4MxEUoJm5j
Z1MSmuJ4q/+w4h6HH5bb+njZNmDTAAMv7tLwkqGj+QTkUpmpRsI3xnVyTK/x
-----END CERTIFICATE-----
)EOF";

const char AWS_CERT_PRIVATE[] PROGMEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEogIBAAKCAQEAr9II11PhLGg/sQQ1cR7L3UXP3gyxjCHma0NaLSQiBt6MncTB
MIa046XBMvjM0TA6hsL4UYH5VJ8bKLTl7yWBaMiJVi7ksaNA/N6Xa3U+cR600fdV
W42Fh6NLvnIlEgWx/lCFdwxIaO9vyk43eh2uisJz21ywe3MJNvYG3zbGM6Hn7/zB
psXnMlV0cTCaeBKGiQ41VVFk6toWb+8XrQKyiehxIvxiPh4sMOrtq93r3o6ujC5a
ChS2SmhDpNv4F3/7TrzIaceJgCPJtetykal/mOPe9TmLO48Ralvc3tb7w9zxJ1vc
BADDbpF/68NtxczG2mu+MBz8G9O4/B51s8NwAQIDAQABAoIBAGBxWNIsu65tPMqv
4VS8rN0ih0RpOBcDHW6nElcCo/uapg2AuOwSaDUbXh/EpCz3RJNZbXbgUdEeOXqZ
yCrnKtY8ckEpFd9vCp3qx6p1NJw+WLd38YV32tAtPcYovLbnEtzKdHt57FpLRMn3
H61HjM+mnICulkwRLPPN/rUASbZV5q0VQIl4bMlA6aCSmtT5S7s3gBTVudR3JGKK
i3OOHYUWx765P3Znf2Nz/fFsKYNH6Qn1t5T8ZaRYS/isPxx48KeXst9NL7BJ0aeO
UFooLNEAl7PJPmmfvgv9kwqERobuLYRfs3V15GVQvGfIYfMbvbHjCaznjZ48/sxl
MZpMgvUCgYEA6OmPA4S/wvzJF7chtI8ir+dmyoiAvXwSpBQ79kJ0xI3pAY8LmJPn
eSoJt5sB9eNhWwvDBZixZa1Yxi/ch3y/6EwBfLNqCM16WcYQLH32jKJBCIjNFOT3
X5EBFTfTEgydkD2nOL6eGVlg/XkZgipmyitXYxLpXov4zGURCUST0fcCgYEAwT+y
qEIw0vc39Jqx3SWPCPBn2eq35WDxWyU1dKXppm+sI+7UGFl5Sofe1HPfIytMklyE
EnloFOWgP1/nPId3OTZNA3fBV44rUjtTxa98Wd0Nx1wH88Wh344HfXhgVOGbSBWg
q87mgAt7e8kwFezAz+4IZwyzPrkpL5czuK1wT8cCgYADdFNWrgVaEv24iFLLpsPQ
8BGqN1QRe2GbZWFuSZYJxEEMcN6vKKJqPaFfQ8FHsFOlUgtWbelTpLqIVfVjXQpL
m7T58H9GGiYSZsmen0+kzlNAmSbRhIPWuwPVGLrcras4lHGCefFGVvkFSWKDd7WG
khX9ti4X5OOPxH/4LzgyCQKBgE4Ghv8Swp3bfXdAooH08TydvICrGZb+vlSbwGqZ
tq4htrJHctFmBugF1f2Xz0dm6yLJQfYnmNOfJ0WPVlEFLSkneI7m4mSUnX81Pwy5
Xw71huOwj/FoIYSe1v1dBiAfe0/uk5HrFvwqJrCDOewQTIVnyXK6GQFBR5TUiuiM
9qTvAoGAPK3/rN+vnZ+hkwCk9/Z9ctuMyEySm51Id/WilXZSIXe1eVD4D2mK28/P
CeX+YEH5TZX7EhLkwn5mAdgND7ibwnY7COvWo7AoKQmXFYsAEsWFrGIOcRGitynH
8MTQmnKqRGj3nmiCURph3Yx1dFI9ZihlcLsfIsPfINAxFKGHQiw=
-----END RSA PRIVATE KEY-----
)EOF";


const char *AWS_IOT_ENDPOINT = "a3cxr9ayydc4np-ats.iot.us-east-1.amazonaws.com";

const int AWS_IOT_PORT = 8883;

//=============================
// TOPICOS
//=============================

const char *TOPICOS_PUBLICAR[] = {

    "senai134/shared/projeto/lampada"};

const int TOTAL_TOPICOS_PUBLICAR = sizeof(TOPICOS_PUBLICAR) / sizeof(TOPICOS_PUBLICAR[0]);

const char *TOPICOS_RECEBER[] = {
    "senai134/shared/projeto/lampada"};
const int TOTAL_TOPICOS_RECEBER = sizeof(TOPICOS_RECEBER) / sizeof(TOPICOS_RECEBER[0]);

//=============================
// DEBUG
//=============================

// 0 = sem mensagens
// 1 = apenas erros
// 2 = todas as mensagens
const int DEBUG_NIVEL_INICIAL = 2;

// Pino usado para forçar todas as mensagens na inicialização
const int PINO_HABILITA_DEBUG_COMPLETO = 4;
