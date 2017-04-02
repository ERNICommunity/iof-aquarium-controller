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
#include <Timer.h>
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
#include <TestFishNotificationAdapter.h>

#define MQTT_SERVER     "iot.eclipse.org"
#define PUBLISH_SUFFIX  "sensor/aquarium-trigger"

SerialCommand*        sCmd = 0;
WiFiClient*           wifiClient   = 0;
MqttClient*           mqttClient   = 0;
FishActuator*         fishActuator = 0;
Configuration*        cfg          = 0;

#define SDA_PIN 4
#define SCL_PIN 5

const byte ledPin = 0; // Pin with LED on Adafruit Huzzah

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
  cfg = new Configuration(new IoF_ConfigurationAdapter(fishActuator));

  //-----------------------------------------------------------------------------
  // MQTT Client
  //-----------------------------------------------------------------------------
  mqttClient = new MqttClient(MQTT_SERVER);
  mqttClient->subscribe(new TestLedMqttSubscriber());
  mqttClient->subscribe(new IofConfigMqttSubscriber(cfg));
  mqttClient->subscribe(new IofTriggerMqttSubscriber(cfg, fishActuator));

  //---------------------------------------------------------------------------
  // Capacitive Sensor
  //---------------------------------------------------------------------------
  new CapSensor(new MyCapSensorAdatper(cfg, fishActuator, mqttClient));
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
