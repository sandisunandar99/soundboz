#include <Arduino.h>
#include "config.h"
#include "wifi_manager.h"

void setup()
{
  Serial.begin(SERIAL_BAUD_RATE);
  delay(100);

  Serial.println("\n==========================================");
  Serial.println("   IOT SOUNDBOX - SYSTEM STARTING         ");
  Serial.println("==========================================");

  initWiFi();
}

void loop()
{
  checkWiFiConnection();
}