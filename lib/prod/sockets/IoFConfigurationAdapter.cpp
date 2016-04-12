/*
 * IoFConfigurationAdapter.cpp
 *
 *  Created on: 11.04.2016
 *      Author: niklausd
 */

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
    char* triggerTopicString;
    sprintf(triggerTopicString, "iof/%s/%s/aquarium-trigger", country, city);
    // TODO: implement plugPublishTopic()
//    m_mqttClient->plugPublishTopic(new MqttPublishTopic(triggerTopicString));
  }

  // set "iof/<country>/<city>/aquarium-trigger" as the topic to publish to when the sensor gets activated
}

void IoF_ConfigurationAdapter::configureFish(unsigned int fishHwId, const char* country, const char* city)
{
  if (!m_fishActuator->isFishConfigured(fishHwId))
  {
    m_fishActuator->addFishAtHwId(fishHwId);
  }

  // TODO
  // subscribe to "iof/<country>/<city>/aquarium-trigger" and assign fishHwId to this topic
}
