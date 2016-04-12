/*
 * MqttTopic.cpp
 *
 *  Created on: 11.04.2016
 *      Author: niklausd
 */

#include "MqttTopic.h"

MqttTopic::MqttTopic(const char* topicString)
: m_topicString(topicString)
, m_next(0)
, m_mqttClient(0)
{ }

MqttTopic::~MqttTopic()
{ }

void MqttTopic::attachMqttClient(MqttClient* mqttClient, MqttTopic* nextMqttTopic)
{
  m_mqttClient = mqttClient;
  m_next = nextMqttTopic;
}

MqttTopic* MqttTopic::next()
{
  return m_next;
}

const char* MqttTopic::topicString() const
{
  return m_topicString;
}

MqttClient* MqttTopic::mqttClient()
{
  return m_mqttClient;
}
