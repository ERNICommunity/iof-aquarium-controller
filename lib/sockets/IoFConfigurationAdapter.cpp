/*
 * IoFConfigurationAdapter.cpp
 *
 *  Created on: 11.04.2016
 *      Author: niklausd
 */

#include <ESP8266WiFi.h>
#include <stdio.h>
#include <FishActuator.h>
#include <IofTriggerPublisher.h>
#include <MqttClient.h>

#include <IoFConfigurationAdapter.h>

IoF_ConfigurationAdapter::IoF_ConfigurationAdapter(FishActuator* fishActuator)
: ConfigurationAdapter()
, m_fishActuator(fishActuator)
, m_triggerPublisher(0)
{ }

IoF_ConfigurationAdapter::~IoF_ConfigurationAdapter()
{ }

void IoF_ConfigurationAdapter::attachTriggerPublisher(IofTriggerPublisher* triggerPublisher)
{
  m_triggerPublisher = triggerPublisher;
}

void IoF_ConfigurationAdapter::getMacAddr(char* macAddr, unsigned int macAddrSize)
{
  strncpy(macAddr, WiFi.macAddress().c_str(), macAddrSize);
  Serial.print("IoF_ConfigurationAdapter::getMacAddr(): ");
  Serial.println(macAddr);
}

void IoF_ConfigurationAdapter::configureFish(unsigned int fishHwId, const char* country, const char* city)
{
  if ((0 != m_fishActuator) && !m_fishActuator->isFishConfigured(fishHwId))
  {
    m_fishActuator->addFishAtHwId(fishHwId);
  }
  if (0 != m_triggerPublisher)
  {
    m_triggerPublisher->notifyConfigChanged();
  }
}
