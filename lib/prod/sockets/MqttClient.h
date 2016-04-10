/*
 * MqttClient.h
 *
 *  Created on: 09.04.2016
 *      Author: scan
 */

#ifndef PROD_SOCKETS_MQTTCLIENT_H_
#define PROD_SOCKETS_MQTTCLIENT_H_

class IoF_WiFiClient;
class FishActuator;
class PubSubClient;
class Timer;

class MqttClient
{
public:
  MqttClient(const char* mqttServerDomain, unsigned short int mqttPort, IoF_WiFiClient* iofWifiClient);
  virtual ~MqttClient();

  void setCallback(void (*callback)(char*, uint8_t*, unsigned int));
  void startupClient();
  void reconnect();
  void subscribe();
  void publishCapTouched();
  void loop();

private:
  IoF_WiFiClient* m_iofWifiClient;
  PubSubClient* m_pubSubClient;
  Timer* m_mqttConnectTimer;
  static const int s_reconnectInterval_ms;
  static const char* capTouchedPublishMsg;
  static const char* capTouchedPayload;

private:  // forbidden functions
  MqttClient(const MqttClient& src);              // copy constructor
  MqttClient& operator = (const MqttClient& src); // assignment operator
};

#endif /* PROD_SOCKETS_MQTTCLIENT_H_ */
