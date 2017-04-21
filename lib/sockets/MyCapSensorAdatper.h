/*
 * MyCapSensorAdatper.h
 *
 *  Created on: 31.03.2017
 *      Author: nid
 */

#ifndef LIB_SOCKETS_MYCAPSENSORADATPER_H_
#define LIB_SOCKETS_MYCAPSENSORADATPER_H_

#include <CapSensor.h>

class Configuration;
class FishActuator;
class MqttClient;

class MyCapSensorAdatper: public CapSensorAdapter
{
public:
  MyCapSensorAdatper(Configuration* cfg, FishActuator* fishActuator, MqttClient* mqttClient);
  virtual ~MyCapSensorAdatper();
  virtual void notifyCapTouched(uint8_t currentTouchValue);

private:
  Configuration* m_cfg;
  FishActuator* m_fishActuator;
  MqttClient* m_mqttClient;

private:
  MyCapSensorAdatper();
  MyCapSensorAdatper(const MyCapSensorAdatper& src);              // copy constructor
  MyCapSensorAdatper& operator = (const MyCapSensorAdatper& src); // assignment operator
};

#endif /* LIB_SOCKETS_MYCAPSENSORADATPER_H_ */
