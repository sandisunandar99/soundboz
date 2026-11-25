#pragma once

void initMQTT();
// Panggil di setiap iterasi loop(); non-blocking, reconnect sendiri tiap MQTT_RECONNECT_INTERVAL.
void checkMQTTConnection();
