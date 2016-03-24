/*
 * test_fishCollection.cpp
 *
 *  Created on: 24.03.2016
 *      Author: niklausd
 */

#include <gtest/gtest.h>

#include "FishCollection.h"
#include "Fish.h"
#include "FishActuator.h"

class TestFishAdapter : public FishNotificationAdapter
{
private:
  unsigned int m_eventFishHwId;
  FishEvent m_event;
  unsigned int m_errorFishHwId;
  FishError m_error;

public:
  const static unsigned int cErrHwId = 9999;

  TestFishAdapter()
  : m_eventFishHwId(cErrHwId)
  , m_event((FishEvent) -1)
  , m_errorFishHwId(cErrHwId)
  , m_error((FishError) -1)
  { }

  void notifyFishEvent(unsigned int fishHwId, FishEvent event)
  {
    if (m_eventFishHwId == cErrHwId)
    {
      FAIL() << "No expected event, got " << event << " for hwId " << fishHwId;
    }
    else
    {
      EXPECT_EQ(m_eventFishHwId, fishHwId);
      EXPECT_EQ(m_event, event);
    }
  }

  void notifyFishError(unsigned int fishHwId, FishError error)
  {
    if (m_errorFishHwId == cErrHwId)
    {
      FAIL() << "No expected error, got " << error << " for hwId " << fishHwId;
    }
    else
    {
      EXPECT_EQ(m_errorFishHwId, fishHwId);
      EXPECT_EQ(m_error, error);
    }
  }

  void expectFishEvent(unsigned int fishHwId, FishEvent event)
  {
    m_errorFishHwId = fishHwId;
    m_event = event;
  }

  void expectFishError(unsigned int fishHwId, FishError error)
  {
    m_errorFishHwId = fishHwId;
    m_error = error;
  }
};

TEST(fishCollection, findFishAtHwIdNotFound)
{
  TestFishAdapter* adapter = new TestFishAdapter();
  FishCollection* collection = new FishCollection();
  collection->attachAdapter(adapter);

  ASSERT_TRUE(collection->findFishByHwId(0) == 0);
}

TEST(fishCollection, addFish)
{
  TestFishAdapter* adapter = new TestFishAdapter();
  FishCollection* collection = new FishCollection();
  collection->attachAdapter(adapter);

  unsigned int hwId = 42;
  collection->addFishAtHwId(hwId);

  ASSERT_FALSE(collection->findFishByHwId(hwId) == 0);
}

