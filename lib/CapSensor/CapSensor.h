#pragma once

#include <stdint.h>

class Timer;
class Adafruit_CAP1188;
class DbgTrace_Port;

//-----------------------------------------------------------------------------

/**
 * Interface for a Toggle Button Adapter to receive status change notifications.
 */
class CapSensorAdapter
{
public:
  virtual void notifyFishActivation() = 0;
  virtual void notifyFishStopAll() = 0;

protected:
  CapSensorAdapter() { }
  virtual ~CapSensorAdapter() { }

private:  // forbidden functions
  CapSensorAdapter(const CapSensorAdapter& src);              // copy constructor
  CapSensorAdapter& operator = (const CapSensorAdapter& src); // assignment operator
};

//-----------------------------------------------------------------------------

class CapSensor
{
public:
  CapSensor(CapSensorAdapter* adapter = 0);

  /**
   * Destructor.
   */
  virtual ~CapSensor();

  /**
   * Retrieve pointer to currently injected adapter object.
   * @return Pointer to currently injected ToggleButtonAdapter object; NULL pointer if none is injected.
   */
  CapSensorAdapter* adapter();

  /**
   * Attach adapter object.
   * @param adapter Pointer to specific ToggleButtonAdapter object; NULL pointer: no status change notification will be sent out.
   */
  void attachAdapter(CapSensorAdapter* adapter);

  /**
   * Retrieve current status.
   * @return  Current status, bit set, each bit represents one CAP sense input pin, active (true) or inactive (false).
   */
  uint8_t getTouchBitset();

  /**
   * Notification method.
   * Called whenever one of the 8 touch capacitor bits has changed its state.
   * @param currentTouchBitset, each bit represents one CAP sense input pin, active (true) or inactive (false).
   */
  void notifyTouched(uint8_t currentTouchBitset);
  
private:
  static const int s_keyPollTime;
  Timer* m_debounceTimer;
  CapSensorAdapter* m_adapter;
  Adafruit_CAP1188* m_cap1188;
  bool m_isCap1188Ready;
  DbgTrace_Port* m_trPort;

private:  // forbidden functions
  CapSensor(const CapSensor& src);              // copy constructor
  CapSensor& operator = (const CapSensor& src); // assignment operator
};
