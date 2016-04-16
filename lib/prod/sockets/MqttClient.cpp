/*
 * MqttClient.cpp
 *
 *  Created on: 09.04.2016
 *      Author: scan
 */

#include <Arduino.h>
#include <stdio.h>
#include <ESP8266WiFi.h>
#include <FishActuator.h>
#include <HardwareSerial.h>
#include <include/wl_definitions.h>
#include <IoF_WiFiClient.h>
#include <MqttClient.h>
#include <PubSubClient.h>
#include <stddef.h>
#include <Timer.h>
#include <WiFiClient.h>
#include <WString.h>

const int  MqttClient::s_reconnectInterval_ms = 1000;


//-----------------------------------------------------------------------------
// MQTT Connect Timer Adapter
//-----------------------------------------------------------------------------

class MyMqttConnectTimerAdapter : public TimerAdapter
{
private:
  MqttClient* m_mqttClient;

public:
  MyMqttConnectTimerAdapter(MqttClient* mqttClient)
  : m_mqttClient(mqttClient)
  { }

  void timeExpired()
  {
    if (0 != m_mqttClient)
    {
      m_mqttClient->reconnect();
    }
  }
};


//-----------------------------------------------------------------------------
// MQTT Client
//-----------------------------------------------------------------------------

MqttClient::MqttClient(const char* mqttServerDomain, unsigned short int mqttPort, IoF_WiFiClient* iofWifiClient, MqttClientAdapter* adapter)
: m_iofWifiClient(iofWifiClient)
, m_adapter(adapter)
, m_pubSubClient(new PubSubClient(mqttServerDomain, mqttPort, *(iofWifiClient->getClient())))
, m_mqttConnectTimer(0)
, m_clientCountry("")
, m_clientCity("")
{ }

MqttClient::~MqttClient()
{
  delete m_iofWifiClient;
  m_iofWifiClient = 0;
  delete m_pubSubClient;
  m_pubSubClient = 0;
  delete m_mqttConnectTimer;
  m_mqttConnectTimer = 0;
}

void MqttClient::attachAdapter(MqttClientAdapter* adapter)
{
  m_adapter = adapter;
}

MqttClientAdapter* MqttClient::adapter()
{
  return m_adapter;
}


void MqttClient::setCallback(void (*callback)(char*, uint8_t*, unsigned int))
{
  m_pubSubClient->setCallback(callback);
}

void MqttClient::startupClient()
{
  if ((0 != m_iofWifiClient) && (0 != m_pubSubClient))
  {
    while (WL_CONNECTED != m_iofWifiClient->getStatus())
    {
      delay(200);
    }
    m_mqttConnectTimer = new Timer(new MyMqttConnectTimerAdapter(this), Timer::IS_RECURRING, s_reconnectInterval_ms);
  }
}

void MqttClient::reconnect()
{
  if ((0 != m_adapter) && (WL_CONNECTED == WiFi.status()) && (!m_pubSubClient->connected()))
  {
    const char* macAddress = m_adapter->getMacAddr();

    Serial.print("Attempting MQTT connection, ID is MAC Address: ");
    Serial.print(macAddress);
    Serial.print(" ... ");
    // Attempt to connect
    if (m_pubSubClient->connect(macAddress))
    {
      Serial.println("connected");
      delay(5000);
      // resubscribe
      subscribe();
    }
    else
    {
      int state = m_pubSubClient->state();
      Serial.print("failed, state: ");
      Serial.print(state == MQTT_CONNECTION_TIMEOUT      ? "CONNECTION_TIMEOUT"      :
                   state == MQTT_CONNECTION_LOST         ? "CONNECTION_LOST"         :
                   state == MQTT_CONNECT_FAILED          ? "CONNECT_FAILED"          :
                   state == MQTT_DISCONNECTED            ? "DISCONNECTED"            :
                   state == MQTT_CONNECTED               ? "CONNECTED"               :
                   state == MQTT_CONNECT_BAD_PROTOCOL    ? "CONNECT_BAD_PROTOCOL"    :
                   state == MQTT_CONNECT_BAD_CLIENT_ID   ? "CONNECT_BAD_CLIENT_ID"   :
                   state == MQTT_CONNECT_UNAVAILABLE     ? "CONNECT_UNAVAILABLE"     :
                   state == MQTT_CONNECT_BAD_CREDENTIALS ? "CONNECT_BAD_CREDENTIALS" :
                   state == MQTT_CONNECT_UNAUTHORIZED    ? "CONNECT_UNAUTHORIZED"    : "UNKNOWN");
      Serial.print(", will try again in ");
      Serial.print(s_reconnectInterval_ms / 1000);
      Serial.println(" second(s)");
    }
  }
}

void MqttClient::subscribe()
{
  //TODO
  m_pubSubClient->subscribe("iof/ch/berne/sensor/aquarium-trigger");
  loop();
  m_pubSubClient->subscribe("iof/ch/zurich/sensor/aquarium-trigger");
  loop();
  m_pubSubClient->subscribe("iof/ch/baar/sensor/aquarium-trigger");
  loop();

  if (0 != m_adapter)
  {
    const char* macAddress = m_adapter->getMacAddr();

    if (0 == strncmp(macAddress, "18:FE:34:DB:50:EF", 17))
    {
      m_pubSubClient->subscribe("iof/config/18:FE:34:DB:50:EF");
    }
    else if (0 == strncmp(macAddress, "5C:CF:7F:07:5E:6A", 17))
    {
      m_pubSubClient->subscribe("iof/config/5C:CF:7F:07:5E:6A");
    }
    else if (0 == strncmp(macAddress, "18:FE:34:DB:4F:C5", 17))
    {
      m_pubSubClient->subscribe("iof/config/18:FE:34:DB:4F:C5");
    }
  }
}

void MqttClient::setPublishInfo(const char* country, const char* city)
{
  m_clientCountry = country;
  m_clientCity = city;
}

void MqttClient::publishCapTouched()
{
  size_t buffSize = 100;
  char pubTopicString[buffSize];
  snprintf(pubTopicString, buffSize, "iof/%s/%s/sensor/aquarium-trigger", m_clientCountry, m_clientCity);
  m_pubSubClient->publish(pubTopicString, m_clientCity);
}

void MqttClient::loop()
{
  m_pubSubClient->loop();
}
