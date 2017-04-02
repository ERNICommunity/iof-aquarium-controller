/*
 * TestFishNotificationAdapter.cpp
 *
 *  Created on: 31.03.2017
 *      Author: nid
 */

#include <TestFishNotificationAdapter.h>

TestFishNotificationAdapter::TestFishNotificationAdapter()
{ }

TestFishNotificationAdapter::~TestFishNotificationAdapter()
{ }

void TestFishNotificationAdapter::notifyFishEvent(unsigned int fishHwId, FishEvent event)
{
  Serial.printf("notifyFishEvent(), fishHwId=%d, event=%s\n", fishHwId,
      event == FishNotificationAdapter::EvtFishActivated ? "FishActivated" :
      event == FishNotificationAdapter::EvtFishAdded     ? "FishAdded    " :
      event == FishNotificationAdapter::EvtFishDeleted   ? "FishDeleted  " :
      event == FishNotificationAdapter::EvtFishStopped   ? "FishStopped  " : "UNKNOWN");
}

void TestFishNotificationAdapter::notifyFishError(unsigned int fishHwId, FishError error)
{
  Serial.printf("notifyFishError(), fishHwId=%d, error=%s\n", fishHwId,
      error == FishNotificationAdapter::ErrFishQueueFull      ? "FishQueueFull      " :
      error == FishNotificationAdapter::ErrFishQueueCorrupt   ? "FishQueueCorrupt   " :
      error == FishNotificationAdapter::ErrFishAlreadyExists  ? "FishAlreadyExists  " :
      error == FishNotificationAdapter::ErrFishBusy           ? "FishBusy           " :
      error == FishNotificationAdapter::ErrFishHwIdOutOfRange ? "FishHwIdOutOfRange " :
      error == FishNotificationAdapter::ErrFishNotFound       ? "FishNotFound       " : "UNKNOWN");
}
