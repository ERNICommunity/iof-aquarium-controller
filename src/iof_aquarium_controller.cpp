// Do not remove the include below
#include "iof_aquarium_controller.h"

#include <Wire.h>
#include <CapSensor.h>
#include <Timer.h>
#include <TimerContext.h>
#include <IoF_WiFiClient.h>
#include <PubSubClient.h>
#include <FishActuator.h>

// Update these with values suitable for your network.
#define WIFI_SSID       "DNNet"
#define WIFI_PWD        "Furz1234"
#define MQTT_SERVER_IP  "iot.eclipse.org"
#define MQTT_PORT       1883

#define OFFICE_COUNTRY  "ch"
#define OFFICE_NAME     "berne"
#define OFFICE_KEY      "iofiof"
#define AQUARIUM_ID     "1001"
#define AQUARIUM_SENSOR "sensor/aquarium-trigger"

#define MY_FISH_ID "1"

IoF_WiFiClient* wifiClient = 0;
PubSubClient* mqttClient = 0;
FishActuator* fishActuator = 0;

#define SDA_PIN 4
#define SCL_PIN 5

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
// WiFi Connection Handler
//-----------------------------------------------------------------------------
void subscribe();
const unsigned long c_wiFiConnectIntervalMillis = 1000;
class WiFiConnectTimerAdapter : public TimerAdapter
{
private:

public:
  WiFiConnectTimerAdapter()
  { }

  void timeExpired()
  {

    //report wifi change

    if ((WL_CONNECTED == WiFi.status()) && (!mqttClient->connected()))
    {
      Serial.print("Attempting MQTT connection... ");
      // Attempt to connect
      if (mqttClient->connect("iofiof-1"))
      {
        Serial.println("connected");
        delay(5000);
        // resubscribe
        subscribe();
      }
      else
      {
        int state = mqttClient->state();
        Serial.print("failed, state: ");
        Serial.print(state == MQTT_CONNECTION_TIMEOUT      ? "CONNECTION_TIMEOUT"      :
                     state == MQTT_CONNECTION_LOST         ? "CONNECTION_LOST"         :
                     state == MQTT_CONNECT_FAILED          ? "CONNECT_FAILED"          :
                     state == MQTT_DISCONNECTED            ? "DISCONNECTED"            :
                     state == MQTT_CONNECTED               ? "CONNECTED"               :
                     state == MQTT_CONNECT_BAD_PROTOCOL    ? "CONNECT_BAD_PROTOCOL"    :
                     state == MQTT_CONNECT_BAD_CLIENT_ID   ? "CONNECT_BAD_CLIENT_ID"   :
                     state == MQTT_CONNECT_UNAVAILABLE     ? "CONNECT_UNAVAILABLE"     :
                     state == MQTT_CONNECT_BAD_CREDENTIALS ? "CONNECT_BAD_CREDENTIALS" :
                     state == MQTT_CONNECT_UNAUTHORIZED    ? "CONNECT_UNAUTHORIZED"    : "UNKNOWN");
        Serial.print(", will try again in ");
        Serial.print(c_wiFiConnectIntervalMillis / 1000);
        Serial.println(" second(s)");
      }
    }
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
    if (0 != (currentTouchValue & 1<<0))
    {
      // now it is time to do something
      Serial.println("Touch down!");
      mqttClient->publish("iof/ch/berne/sensor/aquarium-trigger", MY_FISH_ID);
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
    Serial.print(" ");
  }
  Serial.println();

  if (NULL != fishActuator)
  {
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
      case '4':
        fishId = 3;
        break;
      case '5':
        fishId = 4;
        break;
      case '6':
        fishId = 5;
        break;
      case '7':
        fishId = 6;
        break;
      case '8':
        fishId = 7;
        break;
    }
    fishActuator->activateFish(fishId);
  }
}

//The setup function is called once at startup of the sketch
void setup()
{
  //-----------------------------------------------------------------------------
  // Free Heap Logger
  //-----------------------------------------------------------------------------
  new Timer(new FreeHeapLogTimerAdapter(), Timer::IS_RECURRING, c_freeHeapLogIntervalMillis);

  Wire.begin(SDA_PIN, SCL_PIN);

  Serial.begin(115200);

  Serial.println();
  Serial.println(F("---------------------------------------------"));
  Serial.println(F("Hello from IoF Aquarium Controller!"));
  Serial.println(F("---------------------------------------------"));
  Serial.println();

  //-----------------------------------------------------------------------------
  // WiFi Connection
  //-----------------------------------------------------------------------------
  wifiClient = new IoF_WiFiClient(WIFI_SSID, WIFI_PWD, c_wiFiConnectIntervalMillis);
  wifiClient->begin();
  //-----------------------------------------------------------------------------
  // MQTT Client
  //-----------------------------------------------------------------------------
  mqttClient = new PubSubClient(*(wifiClient->getClient()));
  while (WL_CONNECTED != wifiClient->getStatus())
  {
    delay(200);
  }
  mqttClient->setServer(MQTT_SERVER_IP, MQTT_PORT);
  mqttClient->setCallback(callback);

  //---------------------------------------------------------------------------
  // Fish Actuator
  //---------------------------------------------------------------------------
  fishActuator = new FishActuator(new TestFishNotificationAdapter());
  fishActuator->addFishAtHwId(0);
  fishActuator->addFishAtHwId(1);
  fishActuator->addFishAtHwId(2);

  CapSensor* capSensor = new CapSensor(new MyCapSensorAdatper(fishActuator, 0));
}

void subscribe()
{
  //client.subscribe("iof/ch/berne/sensor/aquarium-trigger");
  mqttClient->subscribe("iof/#");
}

// The loop function is called in an endless loop
void loop()
{
  mqttClient->loop();
  yield();
}
