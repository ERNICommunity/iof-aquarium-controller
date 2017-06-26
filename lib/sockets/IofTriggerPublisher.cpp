/*
 * IofTriggerPublisher.cpp
 *
 *  Created on: 23.06.2017
 *      Author: nid
 */

#include <Configuration.h>
#include <MqttTopic.h>
#include <String.h>

#include <IofTriggerPublisher.h>

IofTriggerPublisher::IofTriggerPublisher(Configuration* cfg)
: m_publisher(0)
, m_cfg(cfg)
{
  if ((0 != m_cfg) && m_cfg->isConfigured())
  {
    m_publisher = new MqttTopicPublisher(String("iof/" + String(m_cfg->country()) + "/" + String(m_cfg->city()) +
                                                "/sensor/aquarium-trigger").c_str(), m_cfg->city());
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
    m_publisher = new MqttTopicPublisher(String("iof/" + String(m_cfg->country()) + "/" + String(m_cfg->city()) +
                                                "/sensor/aquarium-trigger").c_str(), m_cfg->city());
  }
}

void IofTriggerPublisher::publish()
{
  if ((0 != m_publisher) && m_cfg->isConfigured())
  {
    m_publisher->publish();
  }
}
