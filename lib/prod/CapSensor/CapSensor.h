#pragma once

#include <stdint.h>

class Timer;
class Adafruit_CAP1188;

//-----------------------------------------------------------------------------

/**
 * Interface for a Toggle Button Adapter to receive status change notifications.
 */
class CapSensorAdapter
{
public:
  /**
   * Notification method to be implemented by specific adapter.
   * Called whenever one of the 8 touch capacitors is touched
   * @param Current status, bit set, each bit represents one CAP sense input pin, active (true) or inactive (false).
   */
  virtual void notifyCapTouched(uint8_t currentTouchValue) = 0;

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
  uint8_t wasTouched();

  void notifyTouched(uint8_t currentTouchValue);
  
private:
  Timer* m_debounceTimer;
  CapSensorAdapter* m_adapter;
  Adafruit_CAP1188* m_adaCap;
  static const int s_defaultKeyPollTime;
  
private:  // forbidden functions
  CapSensor(const CapSensor& src);              // copy constructor
  CapSensor& operator = (const CapSensor& src); // assignment operator
};
