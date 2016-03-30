/*
 * FishCollection.h
 *
 *  Created on: 21.03.2016
 *      Author: niklausd
 */

#ifndef PROD_FISHACTUATOR_SRC_FISHCOLLECTION_H_
#define PROD_FISHACTUATOR_SRC_FISHCOLLECTION_H_

class FishNotificationAdapter;
class Fish;
class FishHal;
class MotionSequencer;

class FishCollection
{
  friend Fish;
public:
  FishCollection();
  virtual ~FishCollection();

  void attachAdapter(FishNotificationAdapter* adapter);
  FishNotificationAdapter* adapter();

  void addFishAtHwId(unsigned int fishHwId);
  void delFishAtHwId(unsigned int fishHwId);

  void activateFish(unsigned int fishHwId);

//  unsigned long activeTimeMillis();
//  unsigned long restTimeMillis();

  /**
   * Get the Fish object pointer of the Fish object having the provided HW ID.
   * @param fishHwId Fish Hardware Id {0..n-1}
   * @return matchingFish Pointer to the matching Fish object, null pointer if not found
   */
  Fish* findFishByHwId(unsigned int fishHwId);

  FishHal* hal();

  MotionSequencer* motionSequencer();

public:
  /**
   * Get busy state of the Fish Collection.
   * @return isBusy true: A Fish is in action; false: none of the Fish are moving.
   */
  bool isBusy();

private:
  FishNotificationAdapter* m_adapter;
  Fish* m_fish;                         /// Root node of single linked list containing the configured Fish objects.
  FishHal* m_hal;
  MotionSequencer* m_sequencer;
  unsigned long m_activeTimeMillis;     /// Time period for timer running during fish motion activity.
  unsigned long m_restTimeMillis;       /// Time period for timer running during fish rest time after the motion activity has been finished.
  bool m_isBusy;                        /// Busy state of the Fish Collection: true: A Fish is in action; false: none of the Fish is moving.


private:  // forbidden functions
  FishCollection(const FishCollection& src);              // copy constructor
  FishCollection& operator = (const FishCollection& src); // assignment operator
};

#endif /* PROD_FISHACTUATOR_SRC_FISHCOLLECTION_H_ */
