/*
 * IoF_MqttClientAdapter.cpp
 *
 *  Created on: 12.04.2016
 *      Author: niklausd
 */

#include <IoF_MqttClientAdapter.h>
#include <IoF_WiFiClient.h>

IoF_MqttClientAdapter::IoF_MqttClientAdapter(IoF_WiFiClient* wifiClient)
: MqttClientAdapter()
, m_wifiClient(wifiClient)
{ }

IoF_MqttClientAdapter::~IoF_MqttClientAdapter()
{ }

const char* IoF_MqttClientAdapter::getMacAddr()
{
  return m_wifiClient->getMacAddress();
}
