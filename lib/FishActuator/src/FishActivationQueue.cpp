/*
 * FishActivationQueue.cpp
 *
 *  Created on: 04.04.2016
 *      Author: niklausd
 */

#include <FishActivationQueue.h>
#include <Fish.h>

const unsigned int FishActivationQueue::s_defaultSize = 10;

FishActivationQueue::FishActivationQueue(unsigned int size)
: m_array(0)
, m_size(size)
, m_itemCount(0)
{
  m_array = new Fish*[m_size];
  for (unsigned int i = 0; i < m_size; i++)
  {
    m_array[i] = 0;
  }
}

FishActivationQueue::~FishActivationQueue()
{
  delete [] m_array;
  m_array = 0;
}

void FishActivationQueue::enqueue(Fish* fish)
{
  if (m_itemCount > 0)
  {
    for (unsigned int i = m_itemCount ; i > 0; i--)
    {
      m_array[i] = m_array[i-1];
    }
  }
  m_array[0] = fish;
  m_itemCount++;

}

Fish* FishActivationQueue::dequeue()
{
  Fish* fish = 0;   // temporary token

  if (m_itemCount > 0)
  {
    fish = m_array[m_itemCount-1];
    m_itemCount--;
  }
  else
  {
    fish = 0;
  }
  return fish;
}

Fish* FishActivationQueue::read()
{
  Fish* fish = 0;   // temporary token

  if (m_itemCount > 0)
  {
    fish = m_array[m_itemCount-1];
  }
  else
  {
    fish = 0;
  }
  return fish;
}

bool FishActivationQueue::isEmpty()
{
  return (0 == m_itemCount);
}

bool FishActivationQueue::isFull()
{
  return (m_size == m_itemCount);
}

unsigned int FishActivationQueue::itemCount()
{
  return m_itemCount;
}
