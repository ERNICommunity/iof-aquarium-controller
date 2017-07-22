/*
 * TestIofConfigRequestSubscriber.h
 *
 *  Created on: 04.05.2017
 *      Author: nid
 */

#ifndef LIB_SOCKETS_TESTIOFCONFIGREQUESTSUBSCRIBER_H_
#define LIB_SOCKETS_TESTIOFCONFIGREQUESTSUBSCRIBER_H_

#include <MqttTopic.h>

class MqttClient;
class Timer;

class TestIofConfigRequestSubscriber: public MqttTopicSubscriber
{
  friend class PublishTimerAdapter;

public:
  TestIofConfigRequestSubscriber();
  virtual ~TestIofConfigRequestSubscriber();

  const char* getConfig() const;
  const char* getPubTopic() const;

protected:
  bool processMessage();

private:
  Timer* m_publishTimer;
  char* m_config;
  char* m_pubTopic;
  static const unsigned int s_configSize;
  static const unsigned int s_pubTopicSize;

private:
  TestIofConfigRequestSubscriber(const TestIofConfigRequestSubscriber& src);              // copy constructor
  TestIofConfigRequestSubscriber& operator = (const TestIofConfigRequestSubscriber& src); // assignment operator
};

#endif /* LIB_SOCKETS_TESTIOFCONFIGREQUESTSUBSCRIBER_H_ */
