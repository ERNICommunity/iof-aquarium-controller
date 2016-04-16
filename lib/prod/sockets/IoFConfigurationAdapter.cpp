/*
 * IoFConfigurationAdapter.cpp
 *
 *  Created on: 11.04.2016
 *      Author: niklausd
 */

#include <Arduino.h>
#include <stdio.h>
#include <IoF_WiFiClient.h>
#include <FishActuator.h>
#include <MqttClient.h>
#include <MqttTopic.h>

#include <IoFConfigurationAdapter.h>

IoF_ConfigurationAdapter::IoF_ConfigurationAdapter(IoF_WiFiClient* wifiClient, MqttClient* mqttClient, FishActuator* fishActuator)
: ConfigurationAdapter()
, m_wifiClient(wifiClient)
, m_mqttClient(mqttClient)
, m_fishActuator(fishActuator)
{ }

IoF_ConfigurationAdapter::~IoF_ConfigurationAdapter()
{ }

const char* IoF_ConfigurationAdapter::getMacAddr()
{
  return m_wifiClient->getMacAddress();
}

void IoF_ConfigurationAdapter::configureAquarium(const char* country, const char* city)
{
  if (0 != m_mqttClient)
  {
    size_t buffSize = 100;
    char triggerTopicString[buffSize];
    snprintf(triggerTopicString, buffSize, "iof/%s/%s/sensor/aquarium-trigger", country, city);
    Serial.print(F("IoF_ConfigurationAdapter::configureAquarium(): "));
    Serial.println(triggerTopicString);
    m_mqttClient->setPublishInfo(country, city);
  }
}

void IoF_ConfigurationAdapter::configureFish(unsigned int fishHwId, const char* country, const char* city)
{
  if (!m_fishActuator->isFishConfigured(fishHwId-1))
  {
    m_fishActuator->addFishAtHwId(fishHwId-1);
  }
}
