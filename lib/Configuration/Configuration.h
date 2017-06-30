/*
 * Configuration.h
 *
 *  Created on: 10.04.2016
 *      Author: niklausd
 */

#ifndef PROD_CONFIGURATION_CONFIGURATION_H_
#define PROD_CONFIGURATION_CONFIGURATION_H_

#include <ArduinoJson.h>
using namespace ArduinoJson;

//-----------------------------------------------------------------------------

class ConfigurationAdapter
{
public:
  virtual ~ConfigurationAdapter() { }

  /**
   * Get MAC Address.
   * @param macAddr c-String with the MAC address in the form "xx:xx:xx:xx:xx:xx"
   * @param macAddrSize buffer size for the mac address
   */
  virtual void getMacAddr(char* macAddr, unsigned int macAddrSize) = 0;

  /**
   *
   */
  virtual void configureFish(unsigned int fishHwId, const char* country, const char* city) = 0;

  /**
   *
   */
  virtual void notifyConfigDone() = 0;

protected:
  ConfigurationAdapter() { }
private:  // forbidden functions
  ConfigurationAdapter(const ConfigurationAdapter& src);              // copy constructor
  ConfigurationAdapter& operator = (const ConfigurationAdapter& src); // assignment operator
};

//-----------------------------------------------------------------------------

class DbgTrace_Port;

class Configuration
{
public:
  Configuration(ConfigurationAdapter* adapter = 0);
  virtual ~Configuration();

  void attachAdapter(ConfigurationAdapter* adapter);

  void setConfig(const char* json, unsigned int jsonSize);

  /**
   * Get Fish ID by the assigned city.
   * @param city City string addressing the fish to be looked up.
   * @return fishId Fish ID assigned to the city {1..N}, FISH_ID_INVALID if not found.
   */
  unsigned int getFishId(const char* city) const;

  bool isConfigured() const { return m_isConfigured; }

  const char* country() const { return m_country; }
  const char* city()    const { return m_city; }

public:
  static const unsigned int FISH_ID_INVALID;   ///

private:
  static const unsigned int s_maxJsonSize;
  static const unsigned int s_maxNameSize;
  static const unsigned int s_macAddrSize;
  ConfigurationAdapter* m_adapter;
  char* m_json;
  unsigned int m_jsonSize;
  bool m_isConfigured;
  char* m_country;
  char* m_city;
  char* m_macAddr;
  DbgTrace_Port* m_trPort;


private:  // forbidden functions
  Configuration(const Configuration& src);              // copy constructor
  Configuration& operator = (const Configuration& src); // assignment operator
};

#endif /* PROD_CONFIGURATION_CONFIGURATION_H_ */
