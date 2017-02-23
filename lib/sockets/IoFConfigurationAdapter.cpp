/*
 * IoFConfigurationAdapter.cpp
 *
 *  Created on: 11.04.2016
 *      Author: niklausd
 */

#include <ESP8266WiFi.h>
#include <stdio.h>
#include <FishActuator.h>
#include <MqttClient.h>

#include <IoFConfigurationAdapter.h>

IoF_ConfigurationAdapter::IoF_ConfigurationAdapter(MqttClient* mqttClient, FishActuator* fishActuator)
: ConfigurationAdapter()
, m_mqttClient(mqttClient)
, m_fishActuator(fishActuator)
{ }

IoF_ConfigurationAdapter::~IoF_ConfigurationAdapter()
{ }

const char* IoF_ConfigurationAdapter::getMacAddr()
{
  return WiFi.macAddress().c_str();
}

void IoF_ConfigurationAdapter::configureAquarium(const char* country, const char* city)
{
  if (0 != m_mqttClient)
  {
    size_t buffSize = 100;
    char triggerTopicString[buffSize];
    snprintf(triggerTopicString, buffSize, "iof/%s/%s/sensor/aquarium-trigger", country, city);
    Serial.print("IoF_ConfigurationAdapter::configureAquarium(): ");
    Serial.println(triggerTopicString);
  }
}

void IoF_ConfigurationAdapter::configureFish(unsigned int fishHwId, const char* country, const char* city)
{
  if (!m_fishActuator->isFishConfigured(fishHwId))
  {
    m_fishActuator->addFishAtHwId(fishHwId);
  }
}
