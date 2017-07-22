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

void MyCapSensorAdatper::notifyFishActivation()
{
  if (0 != m_triggerPublisher)
  {
    m_triggerPublisher->publish();
  }
}

void MyCapSensorAdatper::notifyFishStopAll()
{
  if (0 != m_fishActuator)
  {
    m_fishActuator->stopFish();
  }
}
