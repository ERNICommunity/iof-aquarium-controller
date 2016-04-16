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
    Serial.print("IoF_ConfigurationAdapter::configureAquarium(): ");
    Serial.print(country);
    Serial.print("/");
    Serial.println(city);
    char triggerTopicString[100];
    sprintf(triggerTopicString, "iof/%s/%s/sensor/aquarium-trigger", country, city);
    Serial.print("IoF_ConfigurationAdapter::configureAquarium(): ");
    Serial.println(triggerTopicString);
    // TODO: MqttClient: configure "iof/<country>/<city>/sensor/aquarium-trigger"
    //       as the topic to publish when the sensor is getting activated
  }
}

void IoF_ConfigurationAdapter::configureFish(unsigned int fishHwId, const char* country, const char* city)
{
  if (!m_fishActuator->isFishConfigured(fishHwId-1))
  {
    m_fishActuator->addFishAtHwId(fishHwId-1);
  }

  // TODO:
  // subscribe to "iof/<country>/<city>/sensor/aquarium-trigger" and assign fishHwId to this topic
}
