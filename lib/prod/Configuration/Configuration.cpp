/*
 * Configuration.cpp
 *
 *  Created on: 10.04.2016
 *      Author: niklausd
 */

#include <DynamicJsonBuffer.hpp>
#include <JsonObject.hpp>
#include <cstring>

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
    const int macAddrLen = 18;
    const char* macAddr = m_adapter->getMacAddr();
    if ((macAddrLen == strlen(aquariumId)) && (0 == strncmp(aquariumId, macAddr, macAddrLen)))
    {
      // config is for this controller
      JsonArray& fishMapping = jsonObjectRoot["fish-mapping"];
      for (unsigned int i = 0; i < fishMapping.size(); i++)
      {
        JsonObject& fish     = fishMapping[i];
        unsigned int fishId  = fish["fish-id"];
        JsonObject& office   = fish["office"];
        const char* country  = office["country"];
        const char* city     = office["city"];
        m_adapter->configureFish(fishId, country, city);
      }

      JsonObject& office     = jsonObjectRoot["office"];
      const char* country    = office["country"];
      const char* city       = office["city"];
      m_adapter->configureAquarium(country, city);
    }
  }
}
