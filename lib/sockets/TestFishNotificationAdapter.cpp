/*
 * TestFishNotificationAdapter.cpp
 *
 *  Created on: 31.03.2017
 *      Author: nid
 */

#include <DbgTracePort.h>
#include <DbgTraceLevel.h>
#include <TestFishNotificationAdapter.h>

TestFishNotificationAdapter::TestFishNotificationAdapter()
: m_trPort(new DbgTrace_Port("fshntfy", DbgTrace_Level::info))
{ }

TestFishNotificationAdapter::~TestFishNotificationAdapter()
{
  delete m_trPort;
  m_trPort = 0;
}

void TestFishNotificationAdapter::notifyFishEvent(unsigned int fishHwId, FishEvent event)
{
  TR_PRINTF(m_trPort, DbgTrace_Level::debug, "notifyFishEvent(), fishHwId=%d, event=%s\n", fishHwId,
            event == FishNotificationAdapter::EvtFishActivated ? "FishActivated" :
            event == FishNotificationAdapter::EvtFishAdded     ? "FishAdded    " :
            event == FishNotificationAdapter::EvtFishDeleted   ? "FishDeleted  " :
            event == FishNotificationAdapter::EvtFishStopped   ? "FishStopped  " : "UNKNOWN");
}

void TestFishNotificationAdapter::notifyFishError(unsigned int fishHwId, FishError error)
{
  TR_PRINTF(m_trPort, DbgTrace_Level::error, "notifyFishError(), fishHwId=%d, error=%s\n", fishHwId,
      error == FishNotificationAdapter::ErrFishQueueFull      ? "FishQueueFull      " :
      error == FishNotificationAdapter::ErrFishQueueCorrupt   ? "FishQueueCorrupt   " :
      error == FishNotificationAdapter::ErrFishAlreadyExists  ? "FishAlreadyExists  " :
      error == FishNotificationAdapter::ErrFishBusy           ? "FishBusy           " :
      error == FishNotificationAdapter::ErrFishHwIdOutOfRange ? "FishHwIdOutOfRange " :
      error == FishNotificationAdapter::ErrFishNotFound       ? "FishNotFound       " : "UNKNOWN");
}
