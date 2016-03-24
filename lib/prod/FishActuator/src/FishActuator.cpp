/*
 * FishActuator.cpp
 *
 *  Created on: 21.03.2016
 *      Author: niklausd
 */

#include "../FishActuator.h"
#include "FishCollection.h"


FishActuator::FishActuator()
: m_fishCollection(new FishCollection())
{ }

FishActuator::~FishActuator()
{
  delete m_fishCollection;
  m_fishCollection = 0;
}

void FishActuator::attachAdapter(FishNotificationAdapter* adapter)
{
  if (0 != m_fishCollection)
  {
    m_fishCollection->attachAdapter(adapter);
  }
}

void FishActuator::addFishAtHwId(unsigned int fishHwId)
{
  if (0 != m_fishCollection)
  {
    m_fishCollection->addFishAtHwId(fishHwId);
  }
}

void FishActuator::delFishAtHwId(unsigned int fishHwId)
{
  if (0 != m_fishCollection)
  {
    m_fishCollection->addFishAtHwId(fishHwId);
  }
}

void FishActuator::activateFish(unsigned int fishHwId)
{
  if (0 != m_fishCollection)
  {
    m_fishCollection->activateFish(fishHwId);
  }
}
