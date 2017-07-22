/*
 * MyCapSensorAdatper.h
 *
 *  Created on: 31.03.2017
 *      Author: nid
 */

#ifndef LIB_SOCKETS_MYCAPSENSORADATPER_H_
#define LIB_SOCKETS_MYCAPSENSORADATPER_H_

#include <CapSensor.h>

class FishActuator;
class IofTriggerPublisher;

class MyCapSensorAdatper: public CapSensorAdapter
{
public:
  MyCapSensorAdatper(FishActuator* fishActuator, IofTriggerPublisher* triggerPublisher);
  virtual ~MyCapSensorAdatper();
  virtual void notifyFishActivation();
  virtual void notifyFishStopAll();

private:
  FishActuator* m_fishActuator;
  IofTriggerPublisher* m_triggerPublisher;

private:
  MyCapSensorAdatper();
  MyCapSensorAdatper(const MyCapSensorAdatper& src);              // copy constructor
  MyCapSensorAdatper& operator = (const MyCapSensorAdatper& src); // assignment operator
};

#endif /* LIB_SOCKETS_MYCAPSENSORADATPER_H_ */
