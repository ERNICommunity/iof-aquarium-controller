/*
 * MyCapSensorAdatper.cpp
 *
 *  Created on: 31.03.2017
 *      Author: nid
 */

#include <Arduino.h>
#include <FishActuator.h>
#include <Configuration.h>
#include <MyCapSensorAdatper.h>
#include <MqttClient.h>

MyCapSensorAdatper::MyCapSensorAdatper(Configuration* cfg, FishActuator* fishActuator, MqttClient* mqttClient)
: m_cfg(cfg)
, m_fishActuator(fishActuator)
, m_mqttClient(mqttClient)
{ }

MyCapSensorAdatper::~MyCapSensorAdatper()
{ }

void MyCapSensorAdatper::notifyCapTouched(uint8_t currentTouchValue)
{
  if (0 != (currentTouchValue & 1<<0))
  {
    // now it is time to do something
    Serial.println("Touch down!");
    if ((0 != m_cfg) && (0 != m_mqttClient))
    {
      if (m_cfg->isConfigured())
      {
        size_t buffSize = 100;
        char pubTopicString[buffSize];
        snprintf(pubTopicString, buffSize, "iof/%s/%s/sensor/aquarium-trigger", m_cfg->country(), m_cfg->city());
        m_mqttClient->publish(pubTopicString, m_cfg->city());
      }
    }
  }
  if (0 != (currentTouchValue & 1<<7))
  {
    if (0 != m_fishActuator)
    {
      m_fishActuator->stopFish();
    }
  }
}
