/*
 * IoF_MqttClientAdapter.cpp
 *
 *  Created on: 12.04.2016
 *      Author: niklausd
 */

#include <IoF_MqttClientAdapter.h>
#include <IoF_WiFiClient.h>
#include <Configuration.h>

IoF_MqttClientAdapter::IoF_MqttClientAdapter(IoF_WiFiClient* wifiClient, Configuration* configuration)
: MqttClientAdapter()
, m_wifiClient(wifiClient)
, m_configuration(configuration)
{ }

IoF_MqttClientAdapter::~IoF_MqttClientAdapter()
{ }

const char* IoF_MqttClientAdapter::getMacAddr()
{
  return m_wifiClient->getMacAddress();
}

bool IoF_MqttClientAdapter::isConfigured() const
{
  return m_configuration->isConfigured();
}
