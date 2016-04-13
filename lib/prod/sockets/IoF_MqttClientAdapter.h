/*
 * IoF_MqttClientAdapter.h
 *
 *  Created on: 12.04.2016
 *      Author: niklausd
 */

#ifndef PROD_SOCKETS_IOF_MQTTCLIENTADAPTER_H_
#define PROD_SOCKETS_IOF_MQTTCLIENTADAPTER_H_

#include <MqttClient.h>

class IoF_WiFiClient;

class IoF_MqttClientAdapter: public MqttClientAdapter
{
public:
  IoF_MqttClientAdapter(IoF_WiFiClient* wifiClient);
  virtual ~IoF_MqttClientAdapter();

  const char* getMacAddr();

private:
  IoF_WiFiClient* m_wifiClient;

private:  // forbidden functions
  IoF_MqttClientAdapter();
  IoF_MqttClientAdapter(const IoF_MqttClientAdapter& src);              // copy constructor
  IoF_MqttClientAdapter& operator = (const IoF_MqttClientAdapter& src); // assignment operator
};

#endif /* PROD_SOCKETS_IOF_MQTTCLIENTADAPTER_H_ */
