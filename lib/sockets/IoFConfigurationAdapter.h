/*
 * IoFConfigurationAdapter.h
 *
 *  Created on: 11.04.2016
 *      Author: niklausd
 */

#ifndef PROD_SOCKETS_IOFCONFIGURATIONADAPTER_H_
#define PROD_SOCKETS_IOFCONFIGURATIONADAPTER_H_

#include <Configuration.h>

class FishActuator;

class IoF_ConfigurationAdapter: public ConfigurationAdapter
{
public:
  IoF_ConfigurationAdapter(FishActuator* fishActuator);
  virtual ~IoF_ConfigurationAdapter();

  const char* getMacAddr();
  void configureFish(unsigned int fishHwId, const char* country, const char* city);

private:
    FishActuator* m_fishActuator;

private:  // forbidden functions
  IoF_ConfigurationAdapter();
  IoF_ConfigurationAdapter(const IoF_ConfigurationAdapter& src);              // copy constructor
  IoF_ConfigurationAdapter& operator = (const IoF_ConfigurationAdapter& src); // assignment operator
};

#endif /* PROD_SOCKETS_IOFCONFIGURATIONADAPTER_H_ */
