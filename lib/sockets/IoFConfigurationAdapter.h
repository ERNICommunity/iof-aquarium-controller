/*
 * IoFConfigurationAdapter.h
 *
 *  Created on: 11.04.2016
 *      Author: niklausd
 */

#ifndef PROD_SOCKETS_IOFCONFIGURATIONADAPTER_H_
#define PROD_SOCKETS_IOFCONFIGURATIONADAPTER_H_

#include <Configuration.h>

class MqttClient;
class FishActuator;

class IoF_ConfigurationAdapter: public ConfigurationAdapter
{
public:
  IoF_ConfigurationAdapter(MqttClient* mqttClient, FishActuator* fishActuator);
  virtual ~IoF_ConfigurationAdapter();

  const char* getMacAddr();
  void configureAquarium(const char* country, const char* city);
  void configureFish(unsigned int fishHwId, const char* country, const char* city);

private:
    MqttClient* m_mqttClient;
    FishActuator* m_fishActuator;

private:  // forbidden functions
  IoF_ConfigurationAdapter();
  IoF_ConfigurationAdapter(const IoF_ConfigurationAdapter& src);              // copy constructor
  IoF_ConfigurationAdapter& operator = (const IoF_ConfigurationAdapter& src); // assignment operator
};

#endif /* PROD_SOCKETS_IOFCONFIGURATIONADAPTER_H_ */
