/*
 * Configuration.cpp
 *
 *  Created on: 10.04.2016
 *      Author: niklausd
 */

#include <DynamicJsonBuffer.hpp>
#include <JsonObject.hpp>
#include <cstring>
#include <Arduino.h>

using namespace ArduinoJson;

#include "Configuration.h"

Configuration::Configuration(ConfigurationAdapter* adapter)
: m_adapter(adapter)
, m_jsonBuffer(new DynamicJsonBuffer())
{ }

Configuration::~Configuration()
{ }

void Configuration::setConfig(char json[])
{
  if (0 != m_adapter)
  {
    JsonObject& jsonObjectRoot = m_jsonBuffer->parseObject(json);

    const char* aquariumId = jsonObjectRoot["aquarium-id"];
    Serial.print("JSON aquarium-id: ");
    Serial.println(aquariumId);
    const int macAddrLen = 17;
    const char* macAddr = m_adapter->getMacAddr();
    Serial.print("WiFi MAC Address: ");
    Serial.println(macAddr);
    if ((macAddrLen == strlen(aquariumId)) && (0 == strncmp(aquariumId, macAddr, macAddrLen)))
    {
      // config is for this controller
      Serial.println("config is for this controller");
      JsonArray& fishMapping = jsonObjectRoot["fish-mapping"];
      for (unsigned int i = 0; i < fishMapping.size(); i++)
      {
        JsonObject& fish     = fishMapping[i];
        unsigned int fishId  = fish["fish-id"];
        JsonObject& office   = fish["office"];
        const char* country  = office["country"];
        const char* city     = office["city"];
        Serial.print("JSON fish-id: ");
        Serial.print(fishId);
        Serial.print(", ");
        Serial.print(country);
        Serial.print("/");
        Serial.println(city);
        m_adapter->configureFish(fishId, country, city);
      }

      JsonObject& office     = jsonObjectRoot["office"];
      const char* country    = office["country"];
      const char* city       = office["city"];
      Serial.print("JSON office: ");
      Serial.print(country);
      Serial.print("/");
      Serial.println(city);
      m_adapter->configureAquarium(country, city);
    }
  }
}
