/*
 * TestIofConfigRequestSubscriber.cpp
 *
 *  Created on: 04.05.2017
 *      Author: nid
 */

#include <ArduinoJson.h>
#include <cstring>
#include <Arduino.h>
#include <MqttClient.h>
#include <Timer.h>
#include <TestIofConfigRequestSubscriber.h>

//-----------------------------------------------------------------------------

class PublishTimerAdapter : public TimerAdapter
{
private:
  TestIofConfigRequestSubscriber* m_configurator;
public:
  PublishTimerAdapter(TestIofConfigRequestSubscriber* configurator)
  : m_configurator(configurator)
  { }

  void timeExpired()
  {
    if (0 != m_configurator)
    {
      MqttTopicPublisher testCfgPublisher(m_configurator->getPubTopic(),
                                          m_configurator->getConfig(),
                                          MqttTopicPublisher::DO_AUTO_PUBLISH);
    }
  }
};

//-----------------------------------------------------------------------------

const unsigned int TestIofConfigRequestSubscriber::s_configSize = 1000;
const unsigned int TestIofConfigRequestSubscriber::s_pubTopicSize = 100;

TestIofConfigRequestSubscriber::TestIofConfigRequestSubscriber()
: MqttTopicSubscriber("iof/config")
, m_publishTimer(new Timer(new PublishTimerAdapter(this), Timer::IS_NON_RECURRING))
, m_config(new char[s_configSize])
, m_pubTopic(new char[s_configSize])
{
  memset(m_config, 0, s_configSize);
  memset(m_pubTopic, 0, s_pubTopicSize);
}

TestIofConfigRequestSubscriber::~TestIofConfigRequestSubscriber()
{
  delete [] m_config;
  m_config = 0;

  delete [] m_pubTopic;
  m_pubTopic = 0;
}

//-----------------------------------------------------------------------------

bool TestIofConfigRequestSubscriber::processMessage()
{
  bool msgHasBeenHandled = false;
  if (isMyTopic() && (0 != getRxMsg()))
  {
    StaticJsonBuffer<1000> jsonBuffer;

    JsonObject& root = jsonBuffer.createObject();
    root["aquarium-id"] = getRxMsg()->getRxMsgString(); //"5C:CF:7F:07:5E:6A";
    JsonArray& fishMappingArray = root.createNestedArray("fish-mapping");

    JsonObject& set1 = fishMappingArray.createNestedObject();
    set1["fish-id"] = "1";
    JsonObject& office1 = set1.createNestedObject("office");
    office1["office-id"] = "1";
    office1["city"] = "Bern";
    office1["country"] = "CH";

    JsonObject& set2 = fishMappingArray.createNestedObject();
    set2["fish-id"] = "2";
    JsonObject& office2 = set2.createNestedObject("office");
    office2["office-id"] = "2";
    office2["city"] = "Zurich";
    office2["country"] = "CH";

    JsonObject& targetOffice = root.createNestedObject("office");
    targetOffice["office-id"] = "2";
    targetOffice["city"] = "Zurich";
    targetOffice["country"] = "CH";

    root.printTo(m_config, s_configSize);

    snprintf(m_pubTopic, s_pubTopicSize, "%s/%s", getTopicString(), getRxMsg()->getRxMsgString());
    Serial.println(m_config);

    while(m_publishTimer->isRunning())
    {
      delay(1);  // wait for completion of a potentially running configuration publish action for another Aquarium ID.
    }
    m_publishTimer->startTimer(10);   // trigger config publishing

    msgHasBeenHandled = true;
  }
}

const char* TestIofConfigRequestSubscriber::getConfig() const
{
  return m_config;
}

const char* TestIofConfigRequestSubscriber::getPubTopic() const
{
  return m_pubTopic;
}
