#include "config.h"

StaticJsonDocument<4096> configDoc;

void initialiseConfig() {
  Serial.println("Loading config");
  Serial.println(configJson);
  DeserializationError err = deserializeJson(configDoc, configJson);
  if (err) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(err.c_str());
  };
  Serial.println("Config loaded");
};
