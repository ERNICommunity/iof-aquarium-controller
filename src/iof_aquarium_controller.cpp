// Do not remove the include below
#include "iof_aquarium_controller.h"

#include <Wire.h>
#include <CapSensor.h>
#include <Timer.h>
#include <TimerContext.h>
#include <DbgCliNode.h>
#include <DbgCliTopic.h>
#include <IoF_WiFiClient.h>
#include <IoF_MqttClientAdapter.h>
#include <MqttClient.h>
#include <FishActuator.h>
#include <SerialCommand.h>
#include <Configuration.h>
#include <IoFConfigurationAdapter.h>

// Update these with values suitable for your network.
#define WIFI_SSID       "DNNet"
#define WIFI_PWD        "Furz1234"
#define MQTT_SERVER_IP  "iot.eclipse.org"
#define MQTT_PORT       1883

#define PUBLISH_SUFFIX  "sensor/aquarium-trigger"

SerialCommand* sCmd = 0;
IoF_WiFiClient* wifiClient = 0;
MqttClient* mqttClient = 0;
FishActuator* fishActuator = 0;
Configuration* cfg = 0;

#define SDA_PIN 4
#define SCL_PIN 5

//-----------------------------------------------------------------------------
// Arduino Cmd I/F
//-----------------------------------------------------------------------------
void dbgCliExecute()
{
  if (0 != DbgCli_Node::RootNode())
  {
    const unsigned int firstArgToHandle = 1;
    const unsigned int maxArgCnt = 10;
    char* args[maxArgCnt];
    char* arg = "dbg";
    unsigned int arg_cnt = 0;
    while ((maxArgCnt > arg_cnt) && (0 != arg))
    {
      args[arg_cnt] = arg;
      arg = sCmd->next();
      arg_cnt++;
    }
    DbgCli_Node::RootNode()->execute(static_cast<unsigned int>(arg_cnt), const_cast<const char**>(args), firstArgToHandle);
  }
}

void sayHello()
{
  char *arg;
  arg = sCmd->next();    // Get the next argument from the SerialCommand object buffer
  if (arg != NULL)       // As long as it exists, take it
  {
    Serial.print("Hello ");
    Serial.println(arg);
  }
  else
  {
    Serial.println("Hello, whoever you are");
  }
}

// This is the default handler, and gets called when no other command matches.
void unrecognized(const char *command)
{
  Serial.println("What?");
}

//-----------------------------------------------------------------------------
// Free Heap Logger
//-----------------------------------------------------------------------------
extern "C"
{
  #include "user_interface.h"
}
const unsigned long c_freeHeapLogIntervalMillis = 10000;
class FreeHeapLogTimerAdapter : public TimerAdapter
{
public:
  void timeExpired()
  {
    Serial.print(millis() / 1000);
    Serial.print(" - Free Heap Size: ");
    Serial.println(system_get_free_heap_size());
  }
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
      mqttClient->publishCapTouched();
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
void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print(F("Message arrived ["));
  Serial.print(topic);
  Serial.print(F("] "));
  for (int i = 0; i < length; i++)
  {
    Serial.print((char) payload[i]);
  }
  Serial.println();

  if (0 == strncmp(topic, "iof/config", strlen("iof/config")))
  {
    Serial.println(F("Config received!"));
    if (0 != cfg)
    {
      cfg->setConfig((char*)payload);
    }
  }
  else
  {
    // check if received topic ends with expected aquarium trigger
    if (0 == strncmp(topic + strlen(topic) - strlen(PUBLISH_SUFFIX), PUBLISH_SUFFIX, strlen(PUBLISH_SUFFIX)))
    {
      unsigned int fishId = 1000;
      //TODO: get Fish ID from payload
      // e.g. fishId = cfg->getFishId((char*)payload);
      if ((0 != fishActuator) && (fishId != 1000))
      {
        Serial.print(F("Aquarium trigger event received! activate fish ID: "));
        Serial.println(fishId);
        fishActuator->activateFish(fishId);
      }
    }
  }
}

//The setup function is called once at startup of the sketch
void setup()
{
  //-----------------------------------------------------------------------------
  // Serial Command Object for Debug CLI
  //-----------------------------------------------------------------------------
  Serial.begin(115200);
  sCmd = new SerialCommand();
  DbgCli_Node::AssignRootNode(new DbgCli_Topic(0, "dbg", "Internet of Fish Aquarium Controller Debug CLI Root Node."));

  // Setup callbacks for SerialCommand commands
  sCmd->addCommand("hello", sayHello);        // Echos the string argument back
  sCmd->addCommand("dbg", dbgCliExecute);
  sCmd->setDefaultHandler(unrecognized);      // Handler for command that isn't matched  (says "What?")

  Serial.println();
  Serial.println(F("---------------------------------------------"));
  Serial.println(F("Hello from IoF Aquarium Controller!"));
  Serial.println(F("---------------------------------------------"));
  Serial.println();

  //-----------------------------------------------------------------------------
  // Free Heap Logger
  //-----------------------------------------------------------------------------
  new Timer(new FreeHeapLogTimerAdapter(), Timer::IS_RECURRING, c_freeHeapLogIntervalMillis);

  Wire.begin(SDA_PIN, SCL_PIN);

  //-----------------------------------------------------------------------------
  // WiFi Connection
  //-----------------------------------------------------------------------------
  wifiClient = new IoF_WiFiClient(WIFI_SSID, WIFI_PWD);
  wifiClient->begin();

  //---------------------------------------------------------------------------
  // Fish Actuator
  //---------------------------------------------------------------------------
  fishActuator = new FishActuator(new TestFishNotificationAdapter());

  //---------------------------------------------------------------------------
  // Capacitive Sensor
  //---------------------------------------------------------------------------
  CapSensor* capSensor = new CapSensor(new MyCapSensorAdatper(fishActuator, 0));

  //-----------------------------------------------------------------------------
  // MQTT Client
  //-----------------------------------------------------------------------------
  mqttClient = new MqttClient(MQTT_SERVER_IP, MQTT_PORT, wifiClient, new IoF_MqttClientAdapter(wifiClient));
  mqttClient->setCallback(callback);
  mqttClient->startupClient();

  //-----------------------------------------------------------------------------
  // Configuration
  //-----------------------------------------------------------------------------
  cfg = new Configuration(new IoF_ConfigurationAdapter(wifiClient, mqttClient, fishActuator));
}

// The loop function is called in an endless loop
void loop()
{
  mqttClient->loop();     // process MQTT protocol
  sCmd->readSerial();     // process serial commands
  yield();                // process Timer expiration evaluation
}
