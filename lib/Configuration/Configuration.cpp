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
#include <DbgTracePort.h>
#include <DbgTraceLevel.h>

using namespace ArduinoJson;

#include "Configuration.h"

const unsigned int Configuration::s_maxJsonSize   = 1000;
const unsigned int Configuration::s_maxNameSize   = 50;
const unsigned int Configuration::s_macAddrSize   = 20;
const unsigned int Configuration::FISH_ID_INVALID = 0;

Configuration::Configuration(ConfigurationAdapter* adapter)
: m_adapter(adapter)
, m_json(new char[s_maxJsonSize+1])
, m_jsonSize(s_maxJsonSize)
, m_isConfigured(false)
, m_country(new char[s_maxNameSize+1])
, m_city(new char[s_maxNameSize+1])
, m_macAddr(new char[s_macAddrSize])
, m_trPort(new DbgTrace_Port("cfg", DbgTrace_Level::debug))
{
  memset(m_json, 0, s_maxJsonSize+1);
  memset(m_country, 0, s_maxNameSize+1);
  memset(m_city, 0, s_maxNameSize+1);
  memset(m_macAddr, 0, s_macAddrSize);

  if (0 != m_adapter)
  {
    m_adapter->getMacAddr(m_macAddr, s_macAddrSize);
    TR_PRINTF(m_trPort, DbgTrace_Level::debug, "Config obj. created (aquarium-id: %s)", m_macAddr);
  }
}

Configuration::~Configuration()
{
  delete [] m_json;
  m_json = 0;

  delete [] m_country;
  m_country = 0;

  delete [] m_city;
  m_city = 0;

  delete [] m_macAddr;
  m_macAddr = 0;
}

void Configuration::setConfig(const char* json, unsigned int jsonSize)
{
  m_isConfigured = false;
  m_jsonSize = jsonSize;
  strncpy(m_json, json, jsonSize);
  TR_PRINTF(m_trPort, DbgTrace_Level::debug, "setConfig(): m_json: %s", m_json);

  if (0 != m_adapter)
  {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& jsonObjectRoot = jsonBuffer.parseObject(json);

    if (!jsonObjectRoot.containsKey("aquarium-id"))
    {
      TR_PRINTF(m_trPort, DbgTrace_Level::error, "ERROR - setConfig(): \"aquarium-id\" is missing in JSON!");
      return;
    }
    const char* aquariumId = jsonObjectRoot["aquarium-id"];
    TR_PRINTF(m_trPort, DbgTrace_Level::debug, "setConfig(): \"aquarium-id\": %s [JSON], %s [self]", aquariumId, m_macAddr);
    if (0 != strncmp(aquariumId, m_macAddr, s_macAddrSize))
    {
      TR_PRINTF(m_trPort, DbgTrace_Level::warning, "WARNING - setConfig(): JSON is not for this controller!");
      return;
    }
    TR_PRINTF(m_trPort, DbgTrace_Level::debug, "setConfig(): JSON is for this controller");

    if (!jsonObjectRoot.containsKey("fish-mapping"))
    {
      TR_PRINTF(m_trPort, DbgTrace_Level::error, "ERROR - setConfig(): \"fish-mapping\" is missing in JSON!");
      return;
    }
    JsonArray& fishMapping = jsonObjectRoot["fish-mapping"];

    for (unsigned int i = 0; i < fishMapping.size(); i++)
    {
      JsonObject& fish = fishMapping[i];

      if (!fish.containsKey("fish-id"))
      {
        TR_PRINTF(m_trPort, DbgTrace_Level::error, "ERROR - setConfig(): \"fish-id\"(fish) is missing in JSON!");
        return;
      }
      unsigned int fishId = fish["fish-id"];

      if (!fish.containsKey("office"))
      {
        TR_PRINTF(m_trPort, DbgTrace_Level::error, "ERROR - setConfig(): \"office\"(fish) is missing in JSON!");
        return;
      }
      JsonObject& office = fish["office"];

      if (!office.containsKey("country"))
      {
        TR_PRINTF(m_trPort, DbgTrace_Level::error, "ERROR - setConfig(): \"country\"(fish.office) is missing in JSON!");
        return;
      }
      const char* country = office["country"];

      if (!office.containsKey("city"))
      {
        TR_PRINTF(m_trPort, DbgTrace_Level::error, "ERROR - setConfig(): \"city\"(fish.office) is missing in JSON!");
        return;
      }
      const char* city = office["city"];

      TR_PRINTF(m_trPort, DbgTrace_Level::debug, "setConfig(): JSON fish-id: %d, country: %s, city: %s", fishId, country, city);
      m_adapter->configureFish(fishId, country, city);
    }

    if (!jsonObjectRoot.containsKey("office"))
    {
      TR_PRINTF(m_trPort, DbgTrace_Level::error, "ERROR - setConfig(): \"office\" is missing in JSON!");
      return;
    }
    JsonObject& office = jsonObjectRoot["office"];

    if (!office.containsKey("country"))
    {
      TR_PRINTF(m_trPort, DbgTrace_Level::error, "ERROR - setConfig(): \"country\"(office) is missing in JSON!");
      return;
    }
    const char* country = office["country"];

    if (!office.containsKey("city"))
    {
      TR_PRINTF(m_trPort, DbgTrace_Level::error, "ERROR - setConfig(): \"city\"(office) is missing in JSON!");
      return;
    }
    const char* city = office["city"];

    strncpy(m_country, country, s_maxNameSize);
    strncpy(m_city, city, s_maxNameSize);
    TR_PRINTF(m_trPort, DbgTrace_Level::debug, "setConfig(): JSON office, country: %s, city: %s", m_country, m_city);

    TR_PRINTF(m_trPort, DbgTrace_Level::debug, "setConfig() end: m_json: %s", m_json);

    m_isConfigured = true;
    m_adapter->notifyConfigDone();
  }
}

unsigned int Configuration::getFishId(const char* city) const
{
  char json[m_jsonSize];
  strncpy(json, m_json, m_jsonSize);

  TR_PRINTF(m_trPort, DbgTrace_Level::debug, "getFishId(): searching for city: %s", city);

  unsigned int retFishId = FISH_ID_INVALID;
  bool foundCity = false;
  if (isConfigured())
  {
    TR_PRINTF(m_trPort, DbgTrace_Level::debug, "getFishId(): isConfigured: %s", isConfigured() ? "true" : "false");

    DynamicJsonBuffer jsonBuffer;
    JsonObject& jsonObjectRoot = jsonBuffer.parseObject(json);
    JsonArray& fishMapping = jsonObjectRoot["fish-mapping"];

    TR_PRINTF(m_trPort, DbgTrace_Level::debug, "getFishId(): fishMapping.size: %d", fishMapping.size());

    for (unsigned int i = 0; ((i < fishMapping.size()) && !foundCity); i++)
    {
      JsonObject& fish     = fishMapping[i];
      unsigned int fishId  = fish["fish-id"];
      JsonObject& office   = fish["office"];
      const char* country  = office["country"];
      const char* fishCity = office["city"];
      foundCity = (0 == strncmp(city, fishCity, strlen(fishCity)));
      TR_PRINTF(m_trPort, DbgTrace_Level::debug, "getFishId(): city to be found: %s, fishCity[%d]: %s - found: %s",
                city, i, fishCity, foundCity ? "true" : "false");
      if (foundCity)
      {
        retFishId = fishId;
      }
    }
  }
  return retFishId;
}
