/*
 * IofTriggerPublisher.h
 *
 *  Created on: 23.06.2017
 *      Author: nid
 */

#ifndef LIB_SOCKETS_IOFTRIGGERPUBLISHER_H_
#define LIB_SOCKETS_IOFTRIGGERPUBLISHER_H_

class Configuration;
class MqttTopicPublisher;
class DbgTrace_Port;

class IofTriggerPublisher
{
public:
  IofTriggerPublisher(Configuration* cfg);
  virtual ~IofTriggerPublisher();

  void notifyConfigChanged();

  void publish();

private:
  MqttTopicPublisher* m_publisher;
  Configuration* m_cfg;
  DbgTrace_Port* m_trPort;

private:
  IofTriggerPublisher();                                            // default constructor
  IofTriggerPublisher(const IofTriggerPublisher& src);              // copy constructor
  IofTriggerPublisher& operator = (const IofTriggerPublisher& src); // assignment operator
};

#endif /* LIB_SOCKETS_IOFTRIGGERPUBLISHER_H_ */
