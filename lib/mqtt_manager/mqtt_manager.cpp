#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "mqtt_manager.h"
#include "config.h"
#include "secrets.h"

// ISRG Root X1 (valid s/d Jun 2035) — root CA Let's Encrypt untuk mqtt-dev.sunandar.web.id.
// Fingerprint SHA256: 96:BC:EC:06:26:49:76:F3:74:60:77:9A:CF:28:C5:A7:CF:E8:A3:C0:AA:E1:1A:8F:FC:EE:05:C0:BD:DF:08:C6
static const char ISRG_ROOT_X1[] = R"PEM(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)PEM";

static WiFiClientSecure secureClient;
static BearSSL::X509List caCert(ISRG_ROOT_X1);
static PubSubClient mqttClient(secureClient);
static unsigned long lastReconnectAttempt = 0;

static void mqttCallback(char* topic, byte* payload, unsigned int length)
{
    Serial.print("[MQTT] Pesan di ");
    Serial.print(topic);
    Serial.print(": ");
    for (unsigned int i = 0; i < length && i < 128; i++)
        Serial.print((char)payload[i]);
    Serial.println();
    // TODO(soundboz): aksi command (mis. putar suara) di sini — logika audio belum ada
}

static bool connectMQTT()
{
    // LWT: broker menerbitkan "offline" (retained) bila perangkat putus di luar perintah
    if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD,
                           MQTT_TOPIC_STATUS, 0, true, "offline"))
    {
        mqttClient.publish(MQTT_TOPIC_STATUS, "online", true);
        mqttClient.subscribe(MQTT_TOPIC_CMD);
        return true;
    }
    return false;
}

void initMQTT()
{
    secureClient.setTrustAnchors(&caCert);
    secureClient.setTimeout(15000); // handshake TLS BearSSL bisa lambat di ESP8266
    mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
    mqttClient.setCallback(mqttCallback);

    // BearSSL menolak cert bila jam perangkat "tahun 1970" — sync NTP dulu.
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    Serial.print("[MQTT] Menunggu sync NTP");
    unsigned long start = millis();
    while (time(nullptr) < 1609459200UL && millis() - start < 10000) // > 2021-01-01
    {
        delay(200);
        Serial.print(".");
    }
    Serial.println(time(nullptr) >= 1609459200UL ? " [OK]" : " [TIMEOUT — reconnect MQTT akan coba lagi]");
}

void checkMQTTConnection()
{
    if (WiFi.status() != WL_CONNECTED)
        return; // tunggu WiFi konek dulu

    if (mqttClient.connected())
    {
        mqttClient.loop();
        return;
    }

    unsigned long now = millis();
    if (now - lastReconnectAttempt < MQTT_RECONNECT_INTERVAL)
        return;
    lastReconnectAttempt = now;

    Serial.print("[MQTT] Menghubungkan ke ");
    Serial.print(MQTT_BROKER);
    Serial.print("... ");

    if (connectMQTT())
    {
        Serial.println("[OK]");
    }
    else
    {
        // PubSubClient state: -2 connect gagal/TLS, -4 timeout, 4 kredensial salah, 5 tidak diotorisasi (ACL)
        Serial.print("[GAGAL] state=");
        Serial.println(mqttClient.state());
    }
}
