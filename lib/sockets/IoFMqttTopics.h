/*
 * IoFMqttTopics.h
 *
 *  Created on: 31.03.2017
 *      Author: nid
 */

#ifndef LIB_SOCKETS_IOFMQTTTOPICS_H_
#define LIB_SOCKETS_IOFMQTTTOPICS_H_

#include <MqttTopic.h>

//-----------------------------------------------------------------------------

class TestLedMqttSubscriber : public MqttTopicSubscriber
{
public:
  TestLedMqttSubscriber();
  virtual ~TestLedMqttSubscriber();
  bool processMessage();

private:
  // forbidden default functions
  TestLedMqttSubscriber& operator = (const TestLedMqttSubscriber& src); // assignment operator
  TestLedMqttSubscriber(const TestLedMqttSubscriber& src);              // copy constructor
};

//-----------------------------------------------------------------------------

class Configuration;

class IofConfigMqttSubscriber : public MqttTopicSubscriber
{
public:
  IofConfigMqttSubscriber(Configuration* config);
  virtual ~IofConfigMqttSubscriber();
  bool processMessage();

private:
  Configuration* m_cfg;

private:
  // forbidden default functions
  IofConfigMqttSubscriber();
  IofConfigMqttSubscriber& operator = (const IofConfigMqttSubscriber& src); // assignment operator
  IofConfigMqttSubscriber(const IofConfigMqttSubscriber& src);              // copy constructor
};

//-----------------------------------------------------------------------------

class FishActuator;

class IofTriggerMqttSubscriber : public MqttTopicSubscriber
{
public:
  IofTriggerMqttSubscriber(Configuration* config, FishActuator* fishActuator);
  virtual ~IofTriggerMqttSubscriber();
  bool processMessage();

private:
  Configuration* m_cfg;
  FishActuator* m_fishActuator;

private:
  // forbidden default functions
  IofTriggerMqttSubscriber();
  IofTriggerMqttSubscriber& operator = (const IofConfigMqttSubscriber& src); // assignment operator
  IofTriggerMqttSubscriber(const IofConfigMqttSubscriber& src);              // copy constructor
};

//-----------------------------------------------------------------------------

#endif /* LIB_SOCKETS_IOFMQTTTOPICS_H_ */
