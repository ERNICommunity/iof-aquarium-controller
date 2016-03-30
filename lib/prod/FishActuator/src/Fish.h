/*
 * Fish.h
 *
 *  Created on: 21.03.2016
 *      Author: niklausd
 */

#ifndef PROD_FISHACTUATOR_SRC_FISH_H_
#define PROD_FISHACTUATOR_SRC_FISH_H_

class Timer;
class FishCollection;
class MotionSequencer;

class Fish
{
  friend FishCollection;
public:
  /**
   * Constructor.
   * @param fishHwId Fish Hardware Id {0..n-1}
   * @param collection Pointer to FishCollection object.
   */
  Fish(unsigned int fishHwId, FishCollection* collection);

  /**
   * Destructor.
   */
  virtual ~Fish();

  /**
   * Activate motion sequence.
   */
  void activateMotion();

  /**
   * Get the Fish HW ID.
   * @return fishHwId Fish Hardware Id {0..n-1}
   */
  unsigned int getFishHwId();

  /**
   * Get FishCollection context.
   * @return fishCollection Pointer to the FishCollection object.
   */
  FishCollection* collection();

  /**
   * Accessor to Action Timer.
   * @return actionTimer Pointer to the Action Timer object.
   */
  Timer* actionTimer();

  /**
   * Accessor to Rest Period Timer.
   * @return restPeriodTimer Pointer to the Rest Period Timer object.
   */
  Timer* restPeriodTimer();

  /**
   * Get busy state of the Fish object.
   * @return isBusy true: the Fish is in action; false: the Fish is not moving.
   */
  bool isBusy();

  /**
   * Get ready state of the Fish object.
   * @return isReady true: the Fish is ready to be triggered; false: the Fish is in action or is waiting for the rest period time to expire.
   */
  bool isReady();

protected:
  /**
   * Get next Fish object pointer out of the linked list containing Fish objects.
   * @return Fish object pointer or 0 if current object is the trailing list element.
   */
  Fish* next();

  /**
   * Set next Fish object of the linked list containing Fish objects.
   * @param fish Fish objects object pointer to be set as the next element of the list.
   */
  void setNext(Fish* fish);

private:
//  Timer* m_actionTimer;               /// Timer running during motion activity.
//  Timer* m_restPeriodTimer;           /// Timer running during rest time after the motion activity has been finished.
  unsigned int m_fishHwId;
  FishCollection* m_collection;
  Fish* m_next;

private:  // forbidden functions
  Fish();                             // default constructor
  Fish(const Fish& src);              // copy constructor
  Fish& operator = (const Fish& src); // assignment operator
};

#endif /* PROD_FISHACTUATOR_SRC_FISH_H_ */
