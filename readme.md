# SoundBoz (NodeMCU ESP8266MOD)

## Struktur Proyek (PlatformIO)
- `include/` : Konfigurasi pin (`config.h`) dan kredensial (`secrets.h`).
- `lib/wifi_manager/` : Modul manajemen koneksi WiFi & auto-reconnect.
- `src/main.cpp` : Program utama.

## Cara Setup
1. Salin `include/secrets.example.h` menjadi `include/secrets.h`.
2. Isi `WIFI_SSID` dan `WIFI_PASSWORD` pada `include/secrets.h`.