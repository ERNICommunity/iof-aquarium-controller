/*
 * FishActivationQueue.h
 *
 *  Created on: 04.04.2016
 *      Author: niklausd
 */

#ifndef PROD_FISHACTUATOR_SRC_FISHACTIVATIONQUEUE_H_
#define PROD_FISHACTUATOR_SRC_FISHACTIVATIONQUEUE_H_

class Fish;

class FishActivationQueue
{
public:
  FishActivationQueue(unsigned int size = s_defaultSize);
  virtual ~FishActivationQueue();

  void enqueue(Fish* fish);
  Fish* dequeue();
  Fish* read();
  bool isEmpty();
  bool isFull();
  unsigned int itemCount();

private:
  Fish** m_array;
  unsigned int m_size;
  unsigned int m_itemCount;
  static const unsigned int s_defaultSize;

private:  // forbidden functions
  FishActivationQueue(const FishActivationQueue& src);              // copy constructor
  FishActivationQueue& operator = (const FishActivationQueue& src); // assignment operator
};

#endif /* PROD_FISHACTUATOR_SRC_FISHACTIVATIONQUEUE_H_ */
