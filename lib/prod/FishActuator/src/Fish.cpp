/*
 * Fish.cpp
 *
 *  Created on: 21.03.2016
 *      Author: niklausd
 */

#include "Fish.h"
#include "FishCollection.h"
#include "FishActuator.h"
#include "FishHal.h"
#include "MotionSequencer.h"
//#include "Timer.h"

//#include <Arduino.h>

//-----------------------------------------------------------------------------

//class ActionTimerAdapter : public TimerAdapter
//{
//private:
//  Fish* m_fish;
//public:
//  ActionTimerAdapter(Fish* fish)
//  : m_fish(fish)
//  { }
//
//  void timeExpired()
//  {
//    Timer* restPeriodTimer = m_fish->restPeriodTimer();
//    if (0 != restPeriodTimer)
//    {
//      m_fish->collection()->setIsBusy(false);
//      m_fish->collection()->adapter()->notifyFishEvent(m_fish->getFishHwId(), FishNotificationAdapter::EvtFishStopped);
//      restPeriodTimer->startTimer(m_fish->collection()->restTimeMillis());
//    }
//  }
//};

//-----------------------------------------------------------------------------

//class RestPeriodTimerAdapter : public TimerAdapter
//{
//private:
//  Fish* m_fish;
//public:
//  RestPeriodTimerAdapter(Fish* fish)
//  : m_fish(fish)
//  { }
//
//  void timeExpired()
//  {
//
//  }
//};

//-----------------------------------------------------------------------------

Fish::Fish(unsigned int fishHwId, FishCollection* collection)
: /*m_actionTimer(new Timer(new ActionTimerAdapter(this), Timer::IS_NON_RECURRING))
, m_restPeriodTimer(new Timer(new RestPeriodTimerAdapter(this), Timer::IS_NON_RECURRING))
,*/ m_fishHwId(fishHwId)
, m_angle(0)
, m_collection(collection)
, m_next(0)
{ }

Fish::~Fish()
{
//  delete m_restPeriodTimer->adapter();
//  m_restPeriodTimer->attachAdapter(0);
//  delete m_restPeriodTimer;
//  m_restPeriodTimer = 0;
//
//  delete m_actionTimer->adapter();
//  m_actionTimer->attachAdapter(0);
//  delete m_actionTimer;
//  m_actionTimer = 0;
}

void Fish::activateMotion()
{
//  unsigned long activeTimeMillis = 5000;
  if (0 != m_collection->adapter())
  {
//    if (isBusy())
//    {
//      m_collection->adapter()->notifyFishError(m_fishHwId, FishNotificationAdapter::ErrFishBusy);
//    }
//    else
    {
      m_collection->adapter()->notifyFishEvent(m_fishHwId, FishNotificationAdapter::EvtFishActivated);
    }
  }
//  if (isReady())
  {
//    m_collection->setIsBusy(true);
//    m_actionTimer->startTimer(m_collection->activeTimeMillis());
    m_collection->motionSequencer()->startSequence(this);
  }
}

unsigned int Fish::getFishHwId()
{
  return m_fishHwId;
}

FishCollection* Fish::collection()
{
  return m_collection;
}

//Timer* Fish::actionTimer()
//{
//  return m_actionTimer;
//}
//
//Timer* Fish::restPeriodTimer()
//{
//  return m_restPeriodTimer;
//}

int Fish::getAngle()
{
  return m_angle;
}

void Fish::setAngle(int angle)
{
  m_angle = angle;
}

bool Fish::isBusy()
{
//  return m_actionTimer->isRunning();
  return m_collection->motionSequencer()->isRunning();
}

bool Fish::isReady()
{
//  return (!m_actionTimer->isRunning() && !m_restPeriodTimer->isRunning());
  return !m_collection->motionSequencer()->isRunning();
}

Fish* Fish::next()
{
  return m_next;
}

void Fish::setNext(Fish* fish)
{
  m_next = fish;
}
