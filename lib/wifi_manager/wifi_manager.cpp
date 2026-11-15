#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "wifi_manager.h"
#include "config.h"
#include "secrets.h"

unsigned long lastCheckTime = 0;

void initWiFi()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    Serial.print("[INFO] Menghubungkan ke WiFi: ");
    Serial.println(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int retryCount = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        retryCount++;

        if (retryCount % 30 == 0)
        {
            Serial.println("\n[WARN] Koneksi lama. Pastikan SSID/Password benar & WiFi 2.4GHz.");
        }
    }

    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("\n\n[OK] KONEKSI WIFI BERHASIL!");
    Serial.print("IP Address : ");
    Serial.println(WiFi.localIP());
    Serial.print("RSSI       : ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm\n");
}

void checkWiFiConnection()
{
    if (millis() - lastCheckTime >= WIFI_CHECK_INTERVAL)
    {
        lastCheckTime = millis();

        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.print("[STATUS OK] IP: ");
            Serial.print(WiFi.localIP());
            Serial.print(" | Sinyal: ");
            Serial.print(WiFi.RSSI());
            Serial.println(" dBm");
        }
        else
        {
            digitalWrite(LED_BUILTIN, HIGH);
            Serial.println("[ERROR] WiFi terputus! Menyambung ulang...");
            WiFi.reconnect();
        }
    }
}