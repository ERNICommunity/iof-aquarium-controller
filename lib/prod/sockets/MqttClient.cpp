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
const unsigned int MqttClient::s_maxCountrySize = 5;
const unsigned int MqttClient::s_maxCitySize = 20;

MqttClient::MqttClient(const char* mqttServerDomain, unsigned short int mqttPort, IoF_WiFiClient* iofWifiClient, MqttClientAdapter* adapter)
: m_iofWifiClient(iofWifiClient)
, m_adapter(adapter)
, m_pubSubClient(new PubSubClient(mqttServerDomain, mqttPort, *(iofWifiClient->getClient())))
, m_mqttConnectTimer(0)
, m_wasConnectedBefore(false)
, m_clientCountry(new char[s_maxCountrySize])
, m_clientCity(new char[s_maxCitySize])
{
  memset(m_clientCountry, 0, s_maxCountrySize);
  memset(m_clientCity, 0, s_maxCitySize);
}

MqttClient::~MqttClient()
{
  delete m_iofWifiClient;
  m_iofWifiClient = 0;
  delete m_pubSubClient;
  m_pubSubClient = 0;
  delete m_mqttConnectTimer;
  m_mqttConnectTimer = 0;
  delete m_clientCountry;
  m_clientCountry = 0;
  delete m_clientCity;
  m_clientCity = 0;
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
  if ((0 != m_adapter) && (WL_CONNECTED == WiFi.status()))
  {
    const char* macAddress = m_adapter->getMacAddr();
    bool currentlyConnected = m_pubSubClient->connected();
    if (m_wasConnectedBefore != currentlyConnected)
    {
      // connection state changed
      m_wasConnectedBefore = currentlyConnected;
      if (currentlyConnected)
      {
        // ... to connected
        Serial.println("connected");
        loop();
        subscribeToConfigTopic(macAddress);
        loop();
        subscribeToAquariumTopic();
        loop();
        if (m_adapter->isConfigured())
        {
          // request configuration, only at startup - not at every reconnection of the client
          publishConfigID(macAddress);
        }
      }
      else
      {
        int state = m_pubSubClient->state();
        Serial.print("MQTT connection failed, state: ");
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
      }
    }
    if (!currentlyConnected)
    {
      // currently not connected => try to connect
      Serial.println();
      Serial.print("Attempting MQTT connection, ID is MAC Address: ");
      Serial.print(macAddress);
      Serial.print(" ... ");
      // Attempt to connect
      m_pubSubClient->connect(macAddress);
    }
  }
}

void MqttClient::subscribeToConfigTopic(const char* macAddress)
{
  if (0 != m_pubSubClient)
  {
    size_t buffSize = 100;
    char configTopicString[buffSize];
    snprintf(configTopicString, buffSize, "iof/config/%s", macAddress);
    m_pubSubClient->subscribe(configTopicString);
    loop();
  }
}

void MqttClient::subscribeToAquariumTopic()
{
  if (0 != m_pubSubClient)
  {
    m_pubSubClient->subscribe("iof/+/+/sensor/aquarium-trigger");
    loop();
  }
}

void MqttClient::publishConfigID(const char* macAddress)
{
  if (0 != m_pubSubClient)
  {
    m_pubSubClient->publish("iof/config", macAddress);
    loop();
  }
}

void MqttClient::setPublishInfo(const char* country, const char* city)
{
  strncpy(m_clientCountry, country, s_maxCountrySize);
  strncpy(m_clientCity, city, s_maxCitySize);
}

void MqttClient::publishCapTouched()
{
  if (0 != m_pubSubClient)
  {
    size_t buffSize = 100;
    char pubTopicString[buffSize];
    snprintf(pubTopicString, buffSize, "iof/%s/%s/sensor/aquarium-trigger", m_clientCountry, m_clientCity);
    m_pubSubClient->publish(pubTopicString, m_clientCity);
    Serial.print(F("MQTT publish: "));
    Serial.print(pubTopicString);
    Serial.print(F(" payload: "));
    Serial.println(m_clientCity);
  }
}

void MqttClient::loop()
{
  m_pubSubClient->loop();
}
