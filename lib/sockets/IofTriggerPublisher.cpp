/*
 * IofTriggerPublisher.cpp
 *
 *  Created on: 23.06.2017
 *      Author: nid
 */

#include <Configuration.h>
#include <MqttTopic.h>
#include <String.h>
#include <DbgTracePort.h>
#include <DbgTraceLevel.h>

#include <IofTriggerPublisher.h>

IofTriggerPublisher::IofTriggerPublisher(Configuration* cfg)
: m_publisher(0)
, m_cfg(cfg)
, m_trPort(new DbgTrace_Port("ioftrgpub", DbgTrace_Level::debug))
{
  if ((0 != m_cfg) && m_cfg->isConfigured())
  {
    size_t len = 100;
    char trigPubTopic[len+1];
    snprintf(trigPubTopic, len, "iof/%s/%s/sensor/aquarium-trigger", m_cfg->country(), m_cfg->city());
    m_publisher = new MqttTopicPublisher(trigPubTopic, m_cfg->city());
    TR_PRINTF(m_trPort, DbgTrace_Level::debug, "m_publisher.topic: %s", m_publisher->getTopicString());
  }
}

IofTriggerPublisher::~IofTriggerPublisher()
{
  delete m_publisher;
  m_publisher = 0;
}

void IofTriggerPublisher::notifyConfigChanged()
{
  if (0 != m_cfg)
  {
    if (0 != m_publisher)
    {
      delete m_publisher;
    }
    unsigned int len = 100;
    char trigPubTopic[len+1];
    snprintf(trigPubTopic, len, "iof/%s/%s/sensor/aquarium-trigger", m_cfg->country(), m_cfg->city());
    m_publisher = new MqttTopicPublisher(trigPubTopic, m_cfg->city());
    TR_PRINTF(m_trPort, DbgTrace_Level::debug, "m_publisher.topic: %s", m_publisher->getTopicString());
  }
}

void IofTriggerPublisher::publish()
{
  if ((0 != m_publisher) && m_cfg->isConfigured())
  {
    m_publisher->publish();
  }
}
