#include <Arduino.h>
#include <Timer.h>
#include <Adafruit_CAP1188.h>
#include <DbgTracePort.h>
#include <DbgTraceLevel.h>
#include <DbgCliTopic.h>
#include <CapSensorDbgCommand.h>
#include <CapSensor.h>

const int  CapSensor::s_keyPollTime = 50;

//-----------------------------------------------------------------------------

class MyDebounceTimerAdatper : public TimerAdapter
{
private:
  CapSensor* m_capSensor;
  uint8_t m_lastTouchBitset;

public:
  MyDebounceTimerAdatper(CapSensor* capSensor)
  : m_capSensor(capSensor)
  , m_lastTouchBitset(0)
  { }

  void timeExpired()
  {
    if (0 != m_capSensor)
    {
      uint8_t currentTouchBitset = m_capSensor->getTouchBitset();

      if (m_lastTouchBitset != currentTouchBitset)
      {
        m_lastTouchBitset = currentTouchBitset;
        if (currentTouchBitset)
        {
          m_capSensor->notifyTouched(currentTouchBitset);
        }
      }
    }
  }
};

//-----------------------------------------------------------------------------

CapSensor::CapSensor(CapSensorAdapter* adapter)
: m_debounceTimer(new Timer(new MyDebounceTimerAdatper(this), Timer::IS_RECURRING, s_keyPollTime))
, m_adapter(adapter)
, m_cap1188(new Adafruit_CAP1188())
, m_isCap1188Ready(false)
, m_trPort(new DbgTrace_Port("capsens", DbgTrace_Level::info))
{ 
   // init cap sensor with i2c
   if (m_cap1188->begin())
   {
     m_cap1188->writeRegister(0x1F, 0x6F);
     m_isCap1188Ready = true;
   }
   else
   {
     TR_PRINTF(m_trPort, DbgTrace_Level::error, "ERROR: CAP1188 HW not found!")
   }

   DbgCli_Topic* capSensorTopic = new DbgCli_Topic(DbgCli_Node::RootNode(), "capsens", "CAP Sensor debug commands");
   new DbgCli_Cmd_CapSensAct(capSensorTopic, this);
   new DbgCli_Cmd_CapSensStop(capSensorTopic, this);

}

CapSensor::~CapSensor()
{
  delete m_debounceTimer->adapter();
  delete m_debounceTimer; m_debounceTimer = 0;
  delete m_cap1188; m_cap1188 = 0;
}

CapSensorAdapter* CapSensor::adapter()
{
  return m_adapter;
}

void CapSensor::attachAdapter(CapSensorAdapter* adapter)
{
  m_adapter = adapter;
}

uint8_t CapSensor::getTouchBitset()
{
  uint8_t touchBitset = 0;
  if ((0 != m_cap1188) && (m_isCap1188Ready))
  {
    touchBitset = m_cap1188->touched();
  }
  return touchBitset;
}

void CapSensor::notifyTouched(uint8_t currentTouchBitset)
{
  if(0 != m_adapter)
  {
    if (0 != (currentTouchBitset & 1<<0))
    {
      m_adapter->notifyFishActivation();
    }
    if (0 != (currentTouchBitset & 1<<7))
    {
      m_adapter->notifyFishStopAll();
    }
  }
}
