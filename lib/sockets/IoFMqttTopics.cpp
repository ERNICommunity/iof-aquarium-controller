/*
 * IoFMqttTopics.cpp
 *
 *  Created on: 31.03.2017
 *      Author: nid
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Configuration.h>
#include <FishActuator.h>
#include <DbgTracePort.h>
#include <DbgTraceLevel.h>
#include <IoFMqttTopics.h>

//-----------------------------------------------------------------------------

TestLedMqttSubscriber::TestLedMqttSubscriber()
: MqttTopicSubscriber("test/led")
, m_trPort(new DbgTrace_Port("mqttled", DbgTrace_Level::debug))
{ }

TestLedMqttSubscriber::~TestLedMqttSubscriber()
{
  delete m_trPort;
  m_trPort = 0;
}

bool TestLedMqttSubscriber::processMessage()
{
  bool msgHasBeenHandled = false;
  MqttRxMsg* rxMsg = getRxMsg();

  if (isMyTopic())
  {
    if (0 != rxMsg)
    {
      // take responsibility
      bool pinState = atoi(rxMsg->getRxMsgString());
      TR_PRINTF(m_trPort, DbgTrace_Level::debug, "LED state: %s", (pinState > 0) ? "on" : "off");
      digitalWrite(BUILTIN_LED, !pinState);  // LED state is inverted on ESP8266
      msgHasBeenHandled = true;
    }
    else
    {
      TR_PRINTF(m_trPort, DbgTrace_Level::error, "rxMsg unavailable!");
    }
  }
  return msgHasBeenHandled;
}

//-----------------------------------------------------------------------------

IofConfigMqttSubscriber::IofConfigMqttSubscriber(Configuration* config)
: MqttTopicSubscriber(String(String("iof/config/") + String(WiFi.macAddress())).c_str())
, m_cfg(config)
, m_trPort(new DbgTrace_Port("mqttiofcfg", DbgTrace_Level::debug))
{ }

IofConfigMqttSubscriber::~IofConfigMqttSubscriber()
{
  delete m_trPort;
  m_trPort = 0;
}

bool IofConfigMqttSubscriber::processMessage()
{
  bool msgHasBeenHandled = false;
  MqttRxMsg* rxMsg = getRxMsg();

  if (isMyTopic())
  {
    if (0 != rxMsg)
    {
      // take responsibility
      Serial.println("");
      if (0 != m_cfg)
      {
        TR_PRINTF(m_trPort, DbgTrace_Level::debug, "Config received! Size: %d", rxMsg->getRxMsgSize());
        m_cfg->setConfig(rxMsg->getRxMsgString(), rxMsg->getRxMsgSize());
      }
    }
    else
    {
      TR_PRINTF(m_trPort, DbgTrace_Level::error, "rxMsg unavailable!");
    }
    msgHasBeenHandled = true;
  }
  return msgHasBeenHandled;
}

//-----------------------------------------------------------------------------

IofTriggerMqttSubscriber::IofTriggerMqttSubscriber(Configuration* config, FishActuator* fishActuator)
: MqttTopicSubscriber("iof/+/+/sensor/aquarium-trigger")
, m_cfg(config)
, m_fishActuator(fishActuator)
, m_trPort(new DbgTrace_Port("ioftrgsub", DbgTrace_Level::debug))
{ }

IofTriggerMqttSubscriber::~IofTriggerMqttSubscriber()
{
  delete m_trPort;
  m_trPort = 0;
}

bool IofTriggerMqttSubscriber::processMessage()
{
  bool msgHasBeenHandled = false;
  MqttRxMsg* rxMsg = getRxMsg();

  TR_PRINTF(m_trPort, DbgTrace_Level::debug, "Message arrived, topic: %s", rxMsg->getRxTopic()->getTopicString());
  TR_PRINTF(m_trPort, DbgTrace_Level::debug, "             own topic: %s (is %smy topic)", getTopicString(), isMyTopic() ? "" : "not ");

  if (isMyTopic())
  {
    if ((0 != rxMsg) && (0 != m_cfg))
    {
      // take responsibility
      // get Fish ID from payload
      unsigned int fishId = m_cfg->getFishId(rxMsg->getRxMsgString());
      if ((0 != m_fishActuator) && (fishId != Configuration::FISH_ID_INVALID))
      {
        TR_PRINTF(m_trPort, DbgTrace_Level::debug, "Activating fish for: %s (fish Id %d)", rxMsg->getRxMsgString(), fishId);
        m_fishActuator->activateFish(fishId);
      }
    }
    else
    {
      TR_PRINTF(m_trPort, DbgTrace_Level::error, "rxMsg unavailable!");
    }
    msgHasBeenHandled = true;
  }
  return msgHasBeenHandled;
}

//-----------------------------------------------------------------------------
