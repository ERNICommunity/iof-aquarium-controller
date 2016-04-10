/*
 * MqttClient.cpp
 *
 *  Created on: 09.04.2016
 *      Author: scan
 */

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
const char* MqttClient::capTouchedPublishMsg = "iof/ch/berne/sensor/aquarium-trigger";
const char* MqttClient::capTouchedPayload = "true";


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

MqttClient::MqttClient(const char* mqttServerDomain, unsigned short int mqttPort, IoF_WiFiClient* iofWifiClient)
: m_iofWifiClient(iofWifiClient)
, m_pubSubClient(new PubSubClient(mqttServerDomain, mqttPort, *(iofWifiClient->getClient())))
, m_mqttConnectTimer(0)
{
}

MqttClient::~MqttClient()
{
  delete m_iofWifiClient;
  m_iofWifiClient = 0;
  delete m_pubSubClient;
  m_pubSubClient = 0;
  delete m_mqttConnectTimer;
  m_mqttConnectTimer = 0;
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
  if ((WL_CONNECTED == WiFi.status()) && (!m_pubSubClient->connected()))
  {
    Serial.print("Attempting MQTT connection... ");
    // Attempt to connect
    if (m_pubSubClient->connect("iofiof-1"))
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
  //client.subscribe("iof/ch/berne/sensor/aquarium-trigger");
  //TODO
  m_pubSubClient->subscribe("iof/#");
}

void MqttClient::publishCapTouched()
{
  Serial.println("Touch down!");
  m_pubSubClient->publish(capTouchedPublishMsg, capTouchedPayload);
}

void MqttClient::loop()
{
  m_pubSubClient->loop();
}
