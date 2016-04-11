/*
 * Configuration.h
 *
 *  Created on: 10.04.2016
 *      Author: niklausd
 */

#ifndef PROD_CONFIGURATION_CONFIGURATION_H_
#define PROD_CONFIGURATION_CONFIGURATION_H_

#include <DynamicJsonBuffer.hpp>
using namespace ArduinoJson;

//-----------------------------------------------------------------------------

class ConfigurationAdapter
{
public:
  virtual ~ConfigurationAdapter() { }

  /**
   * Get MAC Address.
   * @return macAddr c-String with the MAC address in the form "xx:xx:xx:xx:xx:xx" (memory allocated by callee)
   */
  virtual const char* getMacAddr() = 0;

  /**
   *
   */
  virtual void configureAquarium(const char* country, const char* city) = 0;

  /**
   *
   */
  virtual void configureFish(unsigned int fishHwId, const char* country, const char* city) = 0;



protected:
  ConfigurationAdapter() { }
private:  // forbidden functions
  ConfigurationAdapter(const ConfigurationAdapter& src);              // copy constructor
  ConfigurationAdapter& operator = (const ConfigurationAdapter& src); // assignment operator
};

//-----------------------------------------------------------------------------

class Configuration
{
public:
  Configuration(ConfigurationAdapter* adapter = 0);
  virtual ~Configuration();

  void attachAdapter(ConfigurationAdapter* adapter);

  void setConfig(char json[]);

private:
  ConfigurationAdapter* m_adapter;
  DynamicJsonBuffer* m_jsonBuffer;

private:  // forbidden functions
  Configuration(const Configuration& src);              // copy constructor
  Configuration& operator = (const Configuration& src); // assignment operator
};

#endif /* PROD_CONFIGURATION_CONFIGURATION_H_ */
