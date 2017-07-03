/*
 * TestFishNotificationAdapter.h
 *
 *  Created on: 31.03.2017
 *      Author: nid
 */

#ifndef LIB_SOCKETS_TESTFISHNOTIFICATIONADAPTER_H_
#define LIB_SOCKETS_TESTFISHNOTIFICATIONADAPTER_H_

#include <Arduino.h>
#include <FishActuator.h>

class DbgTrace_Port;

/**
 * Fish Event & Error Notification Adapter
 */
class TestFishNotificationAdapter: public FishNotificationAdapter
{
public:
  TestFishNotificationAdapter();
  virtual ~TestFishNotificationAdapter();

  void notifyFishEvent(unsigned int fishHwId, FishEvent event);
  void notifyFishError(unsigned int fishHwId, FishError error);

private:
  DbgTrace_Port* m_trPort;

private:
  TestFishNotificationAdapter(const TestFishNotificationAdapter& src);              // copy constructor
  TestFishNotificationAdapter& operator = (const TestFishNotificationAdapter& src); // assignment operator

};


#endif /* LIB_SOCKETS_TESTFISHNOTIFICATIONADAPTER_H_ */
