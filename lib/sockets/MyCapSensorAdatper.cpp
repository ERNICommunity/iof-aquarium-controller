/*
 * MyCapSensorAdatper.cpp
 *
 *  Created on: 31.03.2017
 *      Author: nid
 */

#include <Arduino.h>
#include <FishActuator.h>
#include <MyCapSensorAdatper.h>
#include <IofTriggerPublisher.h>

MyCapSensorAdatper::MyCapSensorAdatper(FishActuator* fishActuator, IofTriggerPublisher* triggerPublisher)
: m_fishActuator(fishActuator)
, m_triggerPublisher(triggerPublisher)
{ }

MyCapSensorAdatper::~MyCapSensorAdatper()
{ }

void MyCapSensorAdatper::notifyCapTouched(uint8_t currentTouchValue)
{
  if (0 != (currentTouchValue & 1<<0))
  {
    // now it is time to do something
    Serial.println("Touch down!");
    if (0 != m_triggerPublisher)
    {
      m_triggerPublisher->publish();
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
