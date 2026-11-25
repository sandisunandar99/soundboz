#pragma once

#define SERIAL_BAUD_RATE 115200
#define WIFI_CHECK_INTERVAL 10000

// Host/kredensial ada di secrets.h; port & topic di sini.
#define MQTT_PORT 8883
#define MQTT_CLIENT_ID "soundboz-01"
#define MQTT_TOPIC_STATUS "soundboz/status"
#define MQTT_TOPIC_CMD "soundboz/cmd"
#define MQTT_RECONNECT_INTERVAL 5000