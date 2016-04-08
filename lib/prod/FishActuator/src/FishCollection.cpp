/*
 * FishCollection.cpp
 *
 *  Created on: 21.03.2016
 *      Author: niklausd
 */

#include "FishCollection.h"
#include "FishActuator.h"
#include "Fish.h"
#include "FishHal.h"
#include "MotionSequencer.h"
#include <FishActivationQueue.h>
#include <Timer.h>

//-----------------------------------------------------------------------------

class QueueProcessingTimerAdapter : public TimerAdapter
{
private:
  FishCollection* m_fishCollection;

public:
  QueueProcessingTimerAdapter(FishCollection* fishCollection)
  : m_fishCollection(fishCollection)
  { }

  void timeExpired()
  {
    if (!m_fishCollection->queue()->isEmpty() && !m_fishCollection->isBusy())
    {
      Fish* fish = m_fishCollection->queue()->dequeue();
      if (0 != fish)
      {
        fish->activateMotion();
      }
      else
      {
        if (0 != m_fishCollection->adapter())
        {
          m_fishCollection->adapter()->notifyFishError(0, FishNotificationAdapter::ErrFishQueueCorrupt);
        }
      }
    }
  }
};

//-----------------------------------------------------------------------------

const unsigned long FishCollection::s_queueProcessingIntervalMillis = 1000;

FishCollection::FishCollection(FishNotificationAdapter* adapter)
: m_adapter(adapter)
, m_queue(new FishActivationQueue())
, m_queueProcessingTimer(new Timer(new QueueProcessingTimerAdapter(this), Timer::IS_RECURRING, s_queueProcessingIntervalMillis))
, m_hal(new FishHal())
, m_sequencer(new MotionSequencer(this))
, m_fish(0)
, m_isBusy(false)
{ }

FishCollection::~FishCollection()
{
  delete m_sequencer;
  m_sequencer = 0;

  delete m_hal;
  m_hal = 0;

  delete m_queueProcessingTimer->adapter();
  m_queueProcessingTimer->attachAdapter(0);

  delete m_queueProcessingTimer;
  m_queueProcessingTimer = 0;

  delete m_queue;
  m_queue = 0;
}

void FishCollection::attachAdapter(FishNotificationAdapter* adapter)
{
  m_adapter = adapter;
}

FishNotificationAdapter* FishCollection::adapter()
{
  return m_adapter;
}

bool FishCollection::isFishConfigured(unsigned int fishHwId)
{
  return (findFishByHwId(fishHwId) != 0);
}

void FishCollection::addFishAtHwId(unsigned int fishHwId)
{
  bool found = isFishConfigured(fishHwId);

  if (!found)
  {
    Fish* fish = new Fish(fishHwId, this);
    if (0 == m_fish)
    {
      m_fish = fish;
    }
    else
    {
      Fish* next = m_fish;
      while (next->next() != 0)
      {
        next = next->next();
      }
      next->setNext(fish);
    }
  }
  else
  {
    if (0 != m_adapter)
    {
      // ERROR: Fish with the particular HW ID already exists, no new fish created!
      m_adapter->notifyFishError(fishHwId, FishNotificationAdapter::ErrFishAlreadyExists);
    }
  }
}

void FishCollection::delFishAtHwId(unsigned int fishHwId)
{
  Fish* fish = findFishByHwId(fishHwId);

  if (0 != fish)
  {
    if (fish == m_fish)
    {
      fish = m_fish->next();
      delete m_fish;
      m_fish = fish;
    }
    else
    {
      Fish* next = m_fish;
      while (next->next() != fish)
      {
        next = next->next();
      }
      next->setNext(fish->next());
      delete fish;
      fish = 0;
    }
  }
  else
  {
    if (0 != m_adapter)
    {
      // ERROR: Fish with the particular HW ID not found, nothing to delete!
      m_adapter->notifyFishError(fishHwId, FishNotificationAdapter::ErrFishNotFound);
    }
  }
}

void FishCollection::activateFish(unsigned int fishHwId)
{
  Fish* fish = findFishByHwId(fishHwId);
  if (0 != fish)
  {
    if (!m_queue->isFull())
    {
      m_queue->enqueue(fish);
    }
    else
    {
      if (0 != m_adapter)
      {
        // ERROR: Fish with the particular HW ID not found, nothing activated!
        m_adapter->notifyFishError(fishHwId, FishNotificationAdapter::ErrFishQueueFull);
      }
    }
  }
  else
  {
    if (0 != m_adapter)
    {
      // ERROR: Fish with the particular HW ID not found, nothing activated!
      m_adapter->notifyFishError(fishHwId, FishNotificationAdapter::ErrFishNotFound);
    }
  }
}

void FishCollection::stopFish()
{
  m_sequencer->stop();
}

Fish* FishCollection::findFishByHwId(unsigned int fishHwId)
{
  Fish* matchingFish = 0;
  if (0 != m_fish)
  {
    if (m_fish->getFishHwId() == fishHwId)
    {
      matchingFish = m_fish;
    }
    else
    {
      Fish* next = m_fish;
      while ((next->next() != 0) && (next->next()->getFishHwId() != fishHwId))
      {
        next = next->next();
      }
      if (next->next() != 0)
      {
        matchingFish = next->next();
      }
    }
  }
  return matchingFish;
}

bool FishCollection::isBusy()
{
  return m_sequencer->isRunning();
}

FishHal* FishCollection::hal()
{
  return m_hal;
}

MotionSequencer* FishCollection::motionSequencer()
{
  return m_sequencer;
}

FishActivationQueue* FishCollection::queue()
{
  return m_queue;
}
