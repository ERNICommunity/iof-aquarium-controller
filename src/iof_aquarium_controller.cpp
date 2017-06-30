#define MQTT_MAX_PACKET_SIZE 1000

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

// PlatformIO libraries
#include <Adafruit_PWMServoDriver.h> // pio lib install 30,   lib details see https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library
#include <ArduinoJson.h>             // pio lib install 64,   lib details see https://github.com/bblanchon/ArduinoJson
#include <PubSubClient.h>            // pio lib install 89,   lib details see https://github.com/knolleary/PubSubClient
#include <SerialCommand.h>           // pio lib install 173,  lib details see https://github.com/kroimon/Arduino-SerialCommand
#include <ThingSpeak.h>              // pio lib install 550,  lib details see https://github.com/mathworks/thingspeak-arduino
#include <Adafruit_CAP1188.h>        // pio lib install 693,  lib details see https://github.com/adafruit/Adafruit_CAP1188_Library
#include <Timer.h>                   // pio lib install 1699, lib details see 

// private libraries
#include <DbgCliNode.h>
#include <DbgCliTopic.h>
#include <DbgTracePort.h>
#include <DbgTraceContext.h>
#include <DbgTraceOut.h>
#include <DbgPrintConsole.h>
#include <DbgTraceLevel.h>
#include <MqttClient.h>
#include <MqttClientController.h>
#include <PubSubClientWrapper.h>
#include <MqttTopic.h>
#include <CapSensor.h>
#include <Configuration.h>
#include <FishHal.h>
#include <FishActuator.h>
#include <CmdHandler.h>
#include <AppDebug.h>
#include <ProductDebug.h>
#include <IoFConfigurationAdapter.h>
#include <IoFMqttTopics.h>
#include <MyCapSensorAdatper.h>
#include <IofTriggerPublisher.h>
#include <TestFishNotificationAdapter.h>
#include <TestIofConfigRequestSubscriber.h>

#define MQTT_SERVER     "test.mosquitto.org"
#define PUBLISH_SUFFIX  "sensor/aquarium-trigger"

SerialCommand*        sCmd              = 0;
WiFiClient*           wifiClient        = 0;

FishActuator*         fishActuator      = 0;
IofTriggerPublisher*  triggerPublisher  = 0;
Configuration*        config            = 0;

#define SDA_PIN 4
#define SCL_PIN 5

void setup()
{
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, 1);

  setupProdDebugEnv();

  //-----------------------------------------------------------------------------
  // ESP8266 WiFi Client
  //-----------------------------------------------------------------------------
  wifiClient = new WiFiClient();

  //-----------------------------------------------------------------------------
  // ThingSpeak Client
  //-----------------------------------------------------------------------------
  ThingSpeak.begin(*(wifiClient));

  //---------------------------------------------------------------------------
  // Fish Actuator
  //---------------------------------------------------------------------------
  fishActuator = new FishActuator(new TestFishNotificationAdapter());

  //-----------------------------------------------------------------------------
  // Configuration
  //-----------------------------------------------------------------------------
  IoF_ConfigurationAdapter* configAdapter = new IoF_ConfigurationAdapter(fishActuator);
  config = new Configuration(configAdapter);

  //---------------------------------------------------------------------------
  // Trigger Publisher
  //---------------------------------------------------------------------------
  triggerPublisher = new IofTriggerPublisher(config);
  configAdapter->attachTriggerPublisher(triggerPublisher);

  //---------------------------------------------------------------------------
  // Capacitive Sensor
  //---------------------------------------------------------------------------
  new CapSensor(new MyCapSensorAdatper(fishActuator, triggerPublisher));

  //-----------------------------------------------------------------------------
  // MQTT Client
  //-----------------------------------------------------------------------------
  MqttClient.begin(MQTT_SERVER);
  new TestLedMqttSubscriber();
  new TestIofConfigRequestSubscriber();
  new IofConfigMqttSubscriber(config);
  new IofTriggerMqttSubscriber(config, fishActuator);
  new MqttTopicPublisher("iof/config", WiFi.macAddress().c_str(), MqttTopicPublisher::DO_AUTO_PUBLISH);
}

// The loop function is called in an endless loop
void loop()
{
  if (0 != sCmd)
  {
    sCmd->readSerial();   // process serial commands
  }
  MqttClient.loop();      // process MQTT protocol
  yield();                // process Timers
}
