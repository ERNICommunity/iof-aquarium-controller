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

//-----------------------------------------------------------------------------

Fish::Fish(unsigned int fishHwId, FishCollection* collection)
: m_fishHwId(fishHwId)
, m_angle(0)
, m_collection(collection)
, m_next(0)
{ }

Fish::~Fish()
{ }

void Fish::activateMotion()
{
  if (0 != m_collection->adapter())
  {
    m_collection->adapter()->notifyFishEvent(m_fishHwId, FishNotificationAdapter::EvtFishActivated);
  }
  m_collection->motionSequencer()->startSequence(this);
}

unsigned int Fish::getFishHwId()
{
  return m_fishHwId;
}

FishCollection* Fish::collection()
{
  return m_collection;
}

int Fish::getAngle()
{
  return m_angle;
}

void Fish::setAngle(int angle)
{
  m_angle = angle;
}

Fish* Fish::next()
{
  return m_next;
}

void Fish::setNext(Fish* fish)
{
  m_next = fish;
}
