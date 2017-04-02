/*
 * Configuration.cpp
 *
 *  Created on: 10.04.2016
 *      Author: niklausd
 */

//#include <DynamicJsonBuffer.hpp>
//#include <JsonObject.hpp>
#include <ArduinoJson.h>
#include <cstring>
#include <Arduino.h>

using namespace ArduinoJson;

#include "Configuration.h"

const unsigned int Configuration::s_maxJsonSize   = 512;
const unsigned int Configuration::s_maxNameSize   = 50;
const unsigned int Configuration::FISH_ID_INVALID = 0;


Configuration::Configuration(ConfigurationAdapter* adapter)
: m_adapter(adapter)
, m_json(new char[s_maxJsonSize+1])
, m_jsonSize(s_maxJsonSize)
, m_isConfigured(false)
, m_country(new char[s_maxNameSize+1])
, m_city(new char[s_maxNameSize+1])
{
  memset(m_json, 0, s_maxJsonSize+1);
  memset(m_country, 0, s_maxNameSize+1);
  memset(m_city, 0, s_maxNameSize+1);
}

Configuration::~Configuration()
{
  delete [] m_json;
  m_json = 0;

  delete [] m_country;
  m_country = 0;

  delete [] m_city;
  m_city = 0;
}

void Configuration::setConfig(const char* json, unsigned int jsonSize)
{
  m_jsonSize = jsonSize;
  strncpy(m_json, json, jsonSize);

  if (0 != m_adapter)
  {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& jsonObjectRoot = jsonBuffer.parseObject(m_json);

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

      strncpy(m_country, country, s_maxNameSize);
      strncpy(m_city, city, s_maxNameSize);

      m_isConfigured = true;
    }
  }
}

unsigned int Configuration::getFishId(const char* city) const
{
  char json[m_jsonSize];
  strncpy(json, m_json, m_jsonSize);

  unsigned int retFishId = FISH_ID_INVALID;
  bool foundCity = false;
  if (isConfigured())
  {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& jsonObjectRoot = jsonBuffer.parseObject(json);
    JsonArray& fishMapping = jsonObjectRoot["fish-mapping"];
    for (unsigned int i = 0; ((i < fishMapping.size()) && !foundCity); i++)
    {
      JsonObject& fish     = fishMapping[i];
      unsigned int fishId  = fish["fish-id"];
      JsonObject& office   = fish["office"];
      const char* country  = office["country"];
      const char* fishCity = office["city"];
      foundCity = (0 == strncmp(city, fishCity, strlen(fishCity)));
      if (foundCity)
      {
        retFishId = fishId;
      }
    }
  }
  return retFishId;
}
