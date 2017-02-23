#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

// PlatformIO libraries
#include <Adafruit_PWMServoDriver.h> // pio lib install 30,  lib details see https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library
#include <ArduinoJson.h>             // pio lib install 64,  lib details see https://github.com/bblanchon/ArduinoJson
#include <PubSubClient.h>            // pio lib install 89,  lib details see https://github.com/knolleary/PubSubClient
#include <SerialCommand.h>           // pio lib install 173, lib details see https://github.com/kroimon/Arduino-SerialCommand
#include <ThingSpeak.h>              // pio lib install 550, lib details see https://github.com/mathworks/thingspeak-arduino
#include <Adafruit_CAP1188.h>        // pio lib install 693, lib details see https://github.com/adafruit/Adafruit_CAP1188_Library

// private libraries
#include <CapSensor.h>
#include <Timer.h>
#include <DbgCliNode.h>
#include <DbgCliTopic.h>
#include <DbgTracePort.h>
#include <DbgTraceContext.h>
#include <DbgTraceOut.h>
#include <DbgPrintConsole.h>
#include <DbgTraceLevel.h>
#include <ProductDebug.h>
#include <MqttClient.h>
#include <PubSubClientWrapper.h>
#include <MqttMsgHandler.h>
#include <Configuration.h>
#include <IoFConfigurationAdapter.h>
#include <FishHal.h>
#include <FishActuator.h>
#include <CmdHandler.h>


// Sockets
#include <IoFConfigurationAdapter.h>

#define MQTT_SERVER  "iot.eclipse.org"

#define PUBLISH_SUFFIX  "sensor/aquarium-trigger"

SerialCommand*        sCmd = 0;
WiFiClient*           wifiClient   = 0;
MqttClient*           mqttClient   = 0;
FishActuator*         fishActuator = 0;
Configuration*        cfg          = 0;

#define SDA_PIN 4
#define SCL_PIN 5


class TestLedMqttMsgHandler : public MqttMsgHandler
{
public:
  TestLedMqttMsgHandler(const char* topic)
  : MqttMsgHandler(topic)
  { }

  bool processMessage()
  {
    bool msgHasBeenHandled = false;

    if (isMyTopic())
    {
      msgHasBeenHandled = true;

      // take responsibility
      Serial.print("LED test handler, topic: ");
      Serial.print(getTopic());
      Serial.print(", msg: ");
      Serial.println(getRxMsg());

      bool pinState = atoi(getRxMsg());
      digitalWrite(BUILTIN_LED, !pinState);
    }

    return msgHasBeenHandled;
  }

private:
  // forbidden default functions
  TestLedMqttMsgHandler();                                              // default constructor
  TestLedMqttMsgHandler& operator = (const TestLedMqttMsgHandler& src); // assignment operator
  TestLedMqttMsgHandler(const TestLedMqttMsgHandler& src);              // copy constructor
};

//-----------------------------------------------------------------------------
// Fish Event & Error Notification Adapter
//-----------------------------------------------------------------------------
class TestFishNotificationAdapter: public FishNotificationAdapter
{
public:
  void notifyFishEvent(unsigned int fishHwId, FishEvent event)
  {
    Serial.printf("notifyFishEvent(), fishHwId=%d, event=%s\n", fishHwId,
        event == FishNotificationAdapter::EvtFishActivated ? "FishActivated" :
        event == FishNotificationAdapter::EvtFishAdded     ? "FishAdded    " :
        event == FishNotificationAdapter::EvtFishDeleted   ? "FishDeleted  " :
        event == FishNotificationAdapter::EvtFishStopped   ? "FishStopped  " : "UNKNOWN");
  }

  void notifyFishError(unsigned int fishHwId, FishError error)
  {
    Serial.printf("notifyFishError(), fishHwId=%d, error=%s\n", fishHwId,
        error == FishNotificationAdapter::ErrFishQueueFull      ? "FishQueueFull      " :
        error == FishNotificationAdapter::ErrFishQueueCorrupt   ? "FishQueueCorrupt   " :
        error == FishNotificationAdapter::ErrFishAlreadyExists  ? "FishAlreadyExists  " :
        error == FishNotificationAdapter::ErrFishBusy           ? "FishBusy           " :
        error == FishNotificationAdapter::ErrFishHwIdOutOfRange ? "FishHwIdOutOfRange " :
        error == FishNotificationAdapter::ErrFishNotFound       ? "FishNotFound       " : "UNKNOWN");
  }
};

//-----------------------------------------------------------------------------
// Sensor Action Adapter
//-----------------------------------------------------------------------------
class MyCapSensorAdatper: public CapSensorAdapter
{
private:
  FishActuator* m_fishActuator;
  unsigned int m_hwId;
public:
  MyCapSensorAdatper(FishActuator* fishActuator, unsigned int hwId)
  : m_fishActuator(fishActuator)
  , m_hwId(hwId)
  { }

  virtual void notifyCapTouched(uint8_t currentTouchValue)
  {
    if (0 != (currentTouchValue & 1<<0))
    {
      // now it is time to do something
      Serial.println("Touch down!");
      if ((0 != mqttClient) && (0 != cfg))
      {
        if (cfg->isConfigured())
        {
          size_t buffSize = 100;
          char pubTopicString[buffSize];
          snprintf(pubTopicString, buffSize, "iof/%s/%s/sensor/aquarium-trigger", cfg->country(), cfg->city());
          mqttClient->publish(pubTopicString, cfg->city());
        }
      }
    }
    if (0 != (currentTouchValue & 1<<7))
    {
      if (0 != m_fishActuator)
      {
        m_fishActuator->stopFish();
      }
    }
  }
};

//-----------------------------------------------------------------------------
// MQTT callback, when receiving a subscribed topic
//-----------------------------------------------------------------------------
//class IofConfigMqttMsgHandler : public MqttMsgHandler
//{
//public:
//  IofConfigMqttMsgHandler()
//  : MqttMsgHandler("iof/config")
//  { }
//
//  void processMessage()
//  {
//    if (isMyTopic(topic))
//    {
//      // take responsibility
//      char msg[length+1];
//      memcpy(msg, payload, length);
//      msg[length] = 0;
//
//      Serial.print("IofConfigMqttMsgHandler, topic: ");
//      Serial.print(getTopic());
//      Serial.print(", msg: ");
//      Serial.println(msg);
//
//      Serial.println("Config received!");
//      if (0 != cfg)
//      {
//        cfg->setConfig(msg, length+1);
//      }
//    }
//    else if (0 != next())
//    {
//      // delegate
//      next()->handleMessage(topic, payload, length);
//    }
//  }
//
//private:
//  // forbidden default functions
//  IofConfigMqttMsgHandler& operator = (const IofConfigMqttMsgHandler& src); // assignment operator
//  IofConfigMqttMsgHandler(const IofConfigMqttMsgHandler& src);              // copy constructor
//};
//
//class IofTriggerMqttMsgHandler : public MqttMsgHandler
//{
//public:
//  IofTriggerMqttMsgHandler()
//  : MqttMsgHandler("iof/*/*/sensor/aquarium-trigger")
//  { }
//
//  void processMessage()
//  {
//    if (isMyTopic(topic))
//    {
//      // take responsibility
//      char msg[length+1];
//      memcpy(msg, payload, length);
//      msg[length] = 0;
//
//      Serial.print("IofTriggerMqttMsgHandler, topic: ");
//      Serial.print(getTopic());
//      Serial.print(", msg: ");
//      Serial.println(msg);
//
//      if (0 != strstr(topic, PUBLISH_SUFFIX))
//      {
//        // get Fish ID from payload
//        Serial.print("Aquarium trigger event received! activate fish for: ");
//        Serial.println(msg);
//        unsigned int fishId = cfg->getFishId(msg);
//        if ((0 != fishActuator) && (fishId != Configuration::FISH_ID_INVALID))
//        {
//          Serial.print("Aquarium trigger event received! activate fish ID: ");
//          Serial.println(fishId);
//          fishActuator->activateFish(fishId);
//        }
//      }
//    }
//    else if (0 != next())
//    {
//      // delegate
//      next()->handleMessage(topic, payload, length);
//    }
//  }
//
//private:
//  // forbidden default functions
//  IofTriggerMqttMsgHandler& operator = (const IofConfigMqttMsgHandler& src); // assignment operator
//  IofTriggerMqttMsgHandler(const IofConfigMqttMsgHandler& src);              // copy constructor
//};

void setup()
{
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, 1);

  setupDebugEnv();
  //-----------------------------------------------------------------------------
  // ESP8266 WiFi Client
  //-----------------------------------------------------------------------------
  wifiClient = new WiFiClient();

  //---------------------------------------------------------------------------
  // Fish Actuator
  //---------------------------------------------------------------------------
  fishActuator = new FishActuator(new TestFishNotificationAdapter());

  //---------------------------------------------------------------------------
  // Capacitive Sensor
  //---------------------------------------------------------------------------
  new CapSensor(new MyCapSensorAdatper(fishActuator, 0));

  //-----------------------------------------------------------------------------
  // ThingSpeak Client
  //-----------------------------------------------------------------------------
  ThingSpeak.begin(*(wifiClient));

  //-----------------------------------------------------------------------------
  // MQTT Client
  //-----------------------------------------------------------------------------
  mqttClient = new MqttClient(MQTT_SERVER);
  mqttClient->subscribe(new TestLedMqttMsgHandler("/test/led"));

  //-----------------------------------------------------------------------------
  // Configuration
  //-----------------------------------------------------------------------------
  cfg = new Configuration(new IoF_ConfigurationAdapter(mqttClient, fishActuator));
}

// The loop function is called in an endless loop
void loop()
{
  if (0 != sCmd)
  {
    sCmd->readSerial();   // process serial commands
  }
  if (0 != mqttClient)
  {
    mqttClient->loop();   // process MQTT protocol
  }
  yield();                // process Timers
}
