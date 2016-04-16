/*
 * MqttClient.h
 *
 *  Created on: 09.04.2016
 *      Author: scan
 */

#ifndef PROD_SOCKETS_MQTTCLIENT_H_
#define PROD_SOCKETS_MQTTCLIENT_H_

#include <stdint.h>

class IoF_WiFiClient;
class FishActuator;
class PubSubClient;
class Timer;

//-----------------------------------------------------------------------------

class MqttClientAdapter
{
public:
  virtual ~MqttClientAdapter() { }

  /**
   * Get MAC Address.
   * @return macAddr c-String with the MAC address in the form "xx:xx:xx:xx:xx:xx" (memory allocated by callee)
   */
  virtual const char* getMacAddr() = 0;

protected:
  MqttClientAdapter() { }
private:  // forbidden functions
  MqttClientAdapter(const MqttClientAdapter& src);              // copy constructor
  MqttClientAdapter& operator = (const MqttClientAdapter& src); // assignment operator
};

//-----------------------------------------------------------------------------

class MqttClient
{
public:
  MqttClient(const char* mqttServerDomain, unsigned short int mqttPort, IoF_WiFiClient* iofWifiClient, MqttClientAdapter* adapter = 0);
  virtual ~MqttClient();

  void attachAdapter(MqttClientAdapter* adapter);
  MqttClientAdapter* adapter();

  void setCallback(void (*callback)(char*, uint8_t*, unsigned int));
  void startupClient();
  void reconnect();
  void subscribeToConfigTopic(const char* macAddress);
  void subscribeToAquariumTopic();
  void publishConfigID(const char* macAddress);
  void setPublishInfo(const char* country, const char* city);
  void publishCapTouched();
  void loop();

private:
  IoF_WiFiClient* m_iofWifiClient;
  MqttClientAdapter* m_adapter;
  PubSubClient* m_pubSubClient;
  Timer* m_mqttConnectTimer;
  const char* m_clientCountry;
  const char* m_clientCity;
  static const int s_reconnectInterval_ms;

private:  // forbidden functions
  MqttClient(const MqttClient& src);              // copy constructor
  MqttClient& operator = (const MqttClient& src); // assignment operator
};

#endif /* PROD_SOCKETS_MQTTCLIENT_H_ */
