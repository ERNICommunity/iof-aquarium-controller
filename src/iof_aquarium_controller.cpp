// Do not remove the include below
#include "iof_aquarium_controller.h"

#include "FishActuator.h"

#include "CapSensor.h"
#include "Cmd.h"
#include "DbgCliNode.h"
#include "DbgCliCommand.h"
#include "DbgCliTopic.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <Wire.h>

// Update these with values suitable for your network.

#define WIFI_SSID       "IoTAP"
#define WIFI_PWD        "12345678"
#define MQTT_SERVER_IP  "iot.eclipse.org"
#define MQTT_PORT       1883

#define OFFICE_COUNTRY  "ch"
#define OFFICE_NAME     "berne"
#define OFFICE_KEY      "iofiof"
#define AQUARIUM_ID     "1001"
#define AQUARIUM_SENSOR "sensor/aquarium-trigger"

#define MY_FISH_ID "1"

//const char* AQUARIUM_PUB_FEED = "iof/" + OFFICE_COUNTRY + "/" + OFFICE_NAME + "/" + AQUARIUM_ID + "/" + AQUARIUM_SENSOR; //TODO: not working
//const char* AQUARIUM_SUB_FEED = "iof/+/+/+/" + AQUARIUM_SENSOR;

WiFiClient espClient;
PubSubClient client(espClient);
FishActuator* fishActuator;

#define SDA_PIN 4
#define SCL_PIN 5

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
        error == FishNotificationAdapter::ErrFishQueueFull     ? "ErrFishQueueFull   " :
        error == FishNotificationAdapter::ErrFishQueueCorrupt  ? "ErrFishQueueCorrupt" :
        error == FishNotificationAdapter::ErrFishAlreadyExists ? "FishAlreadyExists  " :
        error == FishNotificationAdapter::ErrFishBusy          ? "FishBusy           " :
        error == FishNotificationAdapter::ErrFishNotFound      ? "FishNotFound       " : "UNKNOWN");
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
    if (0 < (currentTouchValue & 1<<0))
    {
      // now it is time to do something
      Serial.println("Touch down!");
      client.publish("iof/ch/berne/sensor/aquarium-trigger", MY_FISH_ID);
    }
    if (0 < (currentTouchValue & 1<<7))
    {
      m_fishActuator->stopFish();
    }
  }
};

//-----------------------------------------------------------------------------
// Arduino Cmd I/F
//-----------------------------------------------------------------------------
void dbgCliExecute(int arg_cnt, char **args)
{
  if (0 != DbgCli_Node::RootNode())
  {
    const unsigned int firstArgToHandle = 1;
    DbgCli_Node::RootNode()->execute(static_cast<unsigned int>(arg_cnt), const_cast<const char**>(args), firstArgToHandle);
  }
}

void hello(int arg_cnt, char **args)
{
  Serial.println("Hello world.");
}

//-----------------------------------------------------------------------------
// MQTT Client subscriber callback
//-----------------------------------------------------------------------------
void callback(char* topic, byte* payload, unsigned int length)
{
  unsigned int fishId = 0;
  Serial.print(F("Message arrived ["));
  Serial.print(topic);
  Serial.print(F("] "));
  for (int i = 0; i < length; i++)
  {
    Serial.print((char) payload[i]);
  }

  if (NULL == fishActuator)
  {
    return;
  }

  switch (payload[0])
  {
    case '1':
      fishId = 0;
      break;
    case '2':
      fishId = 1;
      break;
    case '3':
      fishId = 2;
      break;
  }

  fishActuator->activateFish(fishId);

  Serial.println();
}

//
void setup_wifi()
{

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print(F("Connecting to WiFi:"));
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PWD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println(F("WiFi connected"));
  Serial.println(F("IP address: "));
  Serial.println(WiFi.localIP());
  Serial.println(F("MAC address: "));
  Serial.println(WiFi.macAddress());
}

//The setup function is called once at startup of the sketch
void setup()
{
  Wire.begin(SDA_PIN, SCL_PIN);

  cmdInit(115200); //contains Serial.begin(115200);

  Serial.println();
  Serial.println(F("---------------------------------------------"));
  Serial.println(F("Hello from IoF Aquarium Controller!"));
  Serial.println(F("---------------------------------------------"));
  Serial.println();

  setup_wifi();

  client.setServer(MQTT_SERVER_IP, MQTT_PORT);
  client.setCallback(callback);

  //---------------------------------------------------------------------------
  // Fish Actuator
  //---------------------------------------------------------------------------
  fishActuator = new FishActuator(new TestFishNotificationAdapter());
  fishActuator->addFishAtHwId(0);
  fishActuator->addFishAtHwId(1);
  fishActuator->addFishAtHwId(2);
  CapSensor* capSensor = new CapSensor(new MyCapSensorAdatper(fishActuator, 0));

  //---------------------------------------------------------------------------
  // Debug Cli
  //---------------------------------------------------------------------------
  // adding CLI Commands
  cmdAdd("hello", hello);        // Tsst dummy
  cmdAdd("dbg", dbgCliExecute);  // DbgCli integration

  //---------------------------------------------------------------------------
  // Debug Trace
  //---------------------------------------------------------------------------
//  DbgCli_Topic* traceTopic = new DbgCli_Topic(DbgCli_Node::RootNode(), "tr", "Modify debug trace");
//  DbgTrace_Context* traceContext = new DbgTrace_Context(traceTopic);
//  new DbgTrace_Out(DbgTrace_Context::getContext(), "trConOut", new DbgPrint_Console());
}

void subscribe()
{
  //client.subscribe("iof/ch/berne/sensor/aquarium-trigger");
  client.subscribe("iof/#");
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print(F("Attempting MQTT connection..."));
    // Attempt to connect
    if (client.connect(MY_FISH_ID))
    {
      Serial.println(F("connected"));
      // resubscribe
      subscribe();
      delay(500);
    }
    else
    {
      Serial.print(F("failed, rc="));
      Serial.print(client.state());
      Serial.println(F(" try again in 5 seconds"));
      // Wait 3 seconds before retrying
      delay(5000);
    }
  }
}

// The loop function is called in an endless loop
void loop()
{
  if (!client.connected())
  {
    reconnect();
    delay(100);
  }

  yield();
  client.loop(); //must be called regularly
  cmdPoll();
}
