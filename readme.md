# SoundBoz (NodeMCU ESP8266MOD)

## Struktur Proyek (PlatformIO)
- `include/` : Konfigurasi pin (`config.h`) dan kredensial (`secrets.h`).
- `lib/wifi_manager/` : Modul manajemen koneksi WiFi & auto-reconnect.
- `src/main.cpp` : Program utama.

## Cara Setup
1. Salin `include/secrets.example.h` menjadi `include/secrets.h`.
2. Isi `WIFI_SSID` dan `WIFI_PASSWORD` pada `include/secrets.h`.
3. Isi `MQTT_BROKER`, `MQTT_USERNAME`, `MQTT_PASSWORD`

## MQTT
- Broker dev: `mqtt.server.com:8883` (mqtts/TLS, cert Let's Encrypt, divalidasi via ISRG Root X1 di `lib/mqtt_manager/mqtt_manager.cpp`).
- Kredensial hanya di `secrets.h` (tidak di-commit). 1883 hanya localhost di server; port ESP8266 selalu 8883.