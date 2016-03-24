/*
 * FishCollection.cpp
 *
 *  Created on: 21.03.2016
 *      Author: niklausd
 */

#include "FishCollection.h"
#include "../FishActuator.h"
#include "Fish.h"

FishCollection::FishCollection()
: m_adapter(0)
, m_fish(0)
, m_activeTimeMillis(5000)
, m_restTimeMillis(2000)
, m_isBusy(false)
{ }

FishCollection::~FishCollection()
{ }

void FishCollection::attachAdapter(FishNotificationAdapter* adapter)
{
  m_adapter = adapter;
}

FishNotificationAdapter* FishCollection::adapter()
{
  return m_adapter;
}

unsigned long FishCollection::activeTimeMillis()
{
  return m_activeTimeMillis;
}

unsigned long FishCollection::restTimeMillis()
{
  return m_restTimeMillis;
}

void FishCollection::addFishAtHwId(unsigned int fishHwId)
{
  if (0 == findFishByHwId(fishHwId))
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
      // ERROR: Fish with the particular WH ID already exists, no new fish created!
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
    if (!isBusy())
    {
      fish->activateMotion();
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

Fish* FishCollection::findFishByHwId(unsigned int fishHwId)
{
  Fish* matchingFish = 0;
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
  return matchingFish;
}

bool FishCollection::isBusy()
{
  return false;
}

void FishCollection::setIsBusy(bool isBusy)
{

}

