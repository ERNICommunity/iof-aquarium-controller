/*
 * IoFMqttTopics.cpp
 *
 *  Created on: 31.03.2017
 *      Author: nid
 */

#include <Arduino.h>
#include <Configuration.h>
#include <FishActuator.h>
#include <IoFMqttTopics.h>

//-----------------------------------------------------------------------------

TestLedMqttSubscriber::TestLedMqttSubscriber()
: MqttTopicSubscriber("/test/led")
{ }

TestLedMqttSubscriber::~TestLedMqttSubscriber()
{ }

bool TestLedMqttSubscriber::processMessage()
{
  bool msgHasBeenHandled = false;
  MqttRxMsg* rxMsg = getRxMsg();
  Serial.print("TestLedMqttSubscriber, ");
  Serial.print("isMyTopic(): ");

  if (isMyTopic())
  {
    Serial.print("true");
    if (0 != rxMsg)
    {
      // take responsibility
      Serial.print(", pin state: ");
      bool pinState = atoi(rxMsg->getRxMsg());
      Serial.println(pinState);
      digitalWrite(BUILTIN_LED, !pinState);  // LED state is inverted on ESP8266
      msgHasBeenHandled = true;
    }
    else
    {
      Serial.println("ERROR: rxMsg unavailable!");
    }
  }
  else
  {
    Serial.println("false");
  }
  return msgHasBeenHandled;
}

//-----------------------------------------------------------------------------

IofConfigMqttSubscriber::IofConfigMqttSubscriber(Configuration* config)
: MqttTopicSubscriber("iof/config")
, m_cfg(config)
{ }

IofConfigMqttSubscriber::~IofConfigMqttSubscriber()
{ }

bool IofConfigMqttSubscriber::processMessage()
{
  bool msgHasBeenHandled = false;
  MqttRxMsg* rxMsg = getRxMsg();
  Serial.print("IofConfigMqttSubscriber, ");
  Serial.print("isMyTopic(): ");

  if (isMyTopic())
  {
    Serial.print("true");
    if (0 != rxMsg)
    {
      // take responsibility
      Serial.println("Config received!");
      if (0 != m_cfg)
      {
        m_cfg->setConfig(rxMsg->getRxMsg(), rxMsg->getRxMsgSize());
      }
    }
    msgHasBeenHandled = true;
  }
  return msgHasBeenHandled;
}

//-----------------------------------------------------------------------------

IofTriggerMqttSubscriber::IofTriggerMqttSubscriber(Configuration* config, FishActuator* fishActuator)
: MqttTopicSubscriber("iof/*/*/sensor/aquarium-trigger")
, m_cfg(config)
, m_fishActuator(fishActuator)
{ }

IofTriggerMqttSubscriber::~IofTriggerMqttSubscriber()
{ }

bool IofTriggerMqttSubscriber::processMessage()
{
  bool msgHasBeenHandled = false;
  MqttRxMsg* rxMsg = getRxMsg();
  Serial.print("IofTriggerMqttSubscriber, ");
  Serial.print("isMyTopic(): ");

  if (isMyTopic())
  {
    Serial.print("true");

    if ((0 != rxMsg) && (0 != m_cfg))
    {
      // take responsibility
      Serial.print(" (");
      Serial.print(rxMsg->getRxTopic());
      Serial.println(")");

      // get Fish ID from payload
      Serial.print("Aquarium trigger event received! activate fish for: ");
      Serial.println(rxMsg->getRxMsg());
      unsigned int fishId = m_cfg->getFishId(rxMsg->getRxMsg());
      if ((0 != m_fishActuator) && (fishId != Configuration::FISH_ID_INVALID))
      {
        Serial.print("Aquarium trigger event received! activate fish ID: ");
        Serial.println(fishId);
        m_fishActuator->activateFish(fishId);
      }
    }
    else
    {
      Serial.println();
    }
    msgHasBeenHandled = true;
  }
  return msgHasBeenHandled;
}

//-----------------------------------------------------------------------------
