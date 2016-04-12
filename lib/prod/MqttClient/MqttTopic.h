/*
 * MqttTopic.h
 *
 *  Created on: 11.04.2016
 *      Author: niklausd
 */

#ifndef PROD_MQTTCLIENT_MQTTTOPIC_H_
#define PROD_MQTTCLIENT_MQTTTOPIC_H_

class MqttClient;

class MqttTopic
{
public:
  MqttTopic(const char* topicString);
  virtual ~MqttTopic();

  void attachMqttClient(MqttClient* mqttClient, MqttTopic* nextMqttTopic);
  MqttTopic* next();
  const char* topicString() const;
  MqttClient* mqttClient();

private:
  const char* m_topicString;
  MqttTopic* m_next;
  MqttClient* m_mqttClient;

private:  // forbidden functions
  MqttTopic();
  MqttTopic(const MqttTopic& src);              // copy constructor
  MqttTopic& operator = (const MqttTopic& src); // assignment operator
};

//-----------------------------------------------------------------------------

class MqttPublishTopic : public MqttTopic
{
public:
  MqttPublishTopic(const char* topicString)
  : MqttTopic(topicString)
  { }

  virtual ~MqttPublishTopic() { }

  void publish(const char* payload)
  {
    if (0 != mqttClient())
    {
      // TODO: implement publish in MqttClient
//      mqttClient()->publish(topicString(), payload);
    }
  }

private:  // forbidden functions
  MqttPublishTopic();
  MqttPublishTopic(const MqttPublishTopic& src);              // copy constructor
  MqttPublishTopic& operator = (const MqttPublishTopic& src); // assignment operator
};

//-----------------------------------------------------------------------------

class MqttSubscribeTopic : public MqttTopic
{
public:
  MqttSubscribeTopic(const char* topicString)
  : MqttTopic(topicString)
  { }

  virtual ~MqttSubscribeTopic() { }

  void subscribe()
  {
    if (0 != mqttClient())
    {
      // TODO: implement subscribe in MqttClient with topic string param
//      mqttClient()->subscribe(topicString());
    }
  }

private:  // forbidden functions
  MqttSubscribeTopic();
  MqttSubscribeTopic(const MqttSubscribeTopic& src);              // copy constructor
  MqttSubscribeTopic& operator = (const MqttSubscribeTopic& src); // assignment operator
};

//-----------------------------------------------------------------------------

#endif /* PROD_MQTTCLIENT_MQTTTOPIC_H_ */
