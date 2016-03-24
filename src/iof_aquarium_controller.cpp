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

// Update these with values suitable for your network.

#define WIFI_SSID       "IoTAP"
#define WIFI_PWD        "12345678"
#define MQTT_SERVER_IP  "192.168.43.53"
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

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class TestFishNotificationAdapter : public FishNotificationAdapter
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
        error == FishNotificationAdapter::ErrFishAlreadyExists ? "FishAlreadyExists" :
        error == FishNotificationAdapter::ErrFishBusy          ? "FishBusy         " :
        error == FishNotificationAdapter::ErrFishNotFound      ? "FishNotFound     " : "UNKNOWN");
  }
};

class MyCapSensorAdatper : public CapSensorAdapter
{

public:
  MyCapSensorAdatper()
  { }

  virtual void notifyCapTouched()
  {
    // no it is time to do something
    Serial.println("Touch down!");

    client.publish("iof/ch/berne/sensor/aquarium-trigger", MY_FISH_ID);
  }

};

//-----------------------------------------------------------------------------
// Debugging
//-----------------------------------------------------------------------------
class DbgCli_Command_AddFish : public DbgCli_Command
{
private:
  FishActuator* m_fishActuator;
public:
  DbgCli_Command_AddFish(FishActuator* fishActuator)
  : DbgCli_Command(new DbgCli_Topic(DbgCli_Node::RootNode(), "fish", "Fish test commands"), "add", "Add Fish; .")
  , m_fishActuator(fishActuator)
  { }

  void printUsage()
  {
    Serial.println("dbg fish add - usage: {0..15}");
  }

  void execute(unsigned int argc, const char** args, unsigned int idxToFirstArgToHandle)
  {
    if (argc < 4)
    {
      printUsage();
    }
    else
    {
      unsigned int hwId = atoi(args[idxToFirstArgToHandle]);
      m_fishActuator->addFishAtHwId(hwId);
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

void callback(char* topic, byte* payload, unsigned int length) {
  unsigned int fishId = 0;
  Serial.print(F("Message arrived ["));
  Serial.print(topic);
  Serial.print(F("] "));
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  if(NULL == fishActuator)
  {
     return;
  }

  switch(payload[0])
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

  //fishActuator->activateFish(fishId);

  Serial.println();
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print(F("Connecting to WiFi:"));
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PWD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println(F("WiFi connected"));
  Serial.println(F("IP address: "));
  Serial.println(WiFi.localIP());
}

//The setup function is called once at startup of the sketch
void setup()
{
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
  fishActuator = new FishActuator();
  fishActuator->attachAdapter(new TestFishNotificationAdapter());

  CapSensor* capSensor = new CapSensor(new MyCapSensorAdatper());
  //---------------------------------------------------------------------------
  // Debug Cli
  //---------------------------------------------------------------------------
  DbgCli_Node::AssignRootNode(new DbgCli_Topic(0, "dbg", "Aquarium Controller Debug CLI Root Node."));
  //  new DbgCli_Command_FreeRam();
  new DbgCli_Command_AddFish(fishActuator);
  // adding CLI Commands
  cmdAdd("hello", hello);
  cmdAdd("dbg", dbgCliExecute);

  //---------------------------------------------------------------------------
  // Debug Trace
  //---------------------------------------------------------------------------

//  DbgCli_Topic* traceTopic = new DbgCli_Topic(DbgCli_Node::RootNode(), "tr", "Modify debug trace");
//  DbgTrace_Context* traceContext = new DbgTrace_Context(traceTopic);
//  new DbgTrace_Out(DbgTrace_Context::getContext(), "trConOut", new DbgPrint_Console());
}

void subscribe()
{
  client.subscribe("iof/+/+/sensor/aquarium-trigger");
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print(F("Attempting MQTT connection..."));
    // Attempt to connect
    if (client.connect(AQUARIUM_ID)) {
      Serial.println(F("connected"));
      // resubscribe
      subscribe();
    } else {
      Serial.print(F("failed, rc="));
      Serial.print(client.state());
      Serial.println(F(" try again in 5 seconds"));
      // Wait 3 seconds before retrying
      delay(3000);
    }
  }
}


// The loop function is called in an endless loop
void loop()
{
  if (!client.connected()) {
      reconnect();
   }

  yield();
  client.loop(); //must be called regularly
  cmdPoll();
}
