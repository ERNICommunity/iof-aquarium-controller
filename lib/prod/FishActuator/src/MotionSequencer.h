/*
 * MotionSequencer.h
 *
 *  Created on: 28.03.2016
 *      Author: nid
 */

#ifndef PROD_FISHACTUATOR_SRC_MOTIONSEQUENCER_H_
#define PROD_FISHACTUATOR_SRC_MOTIONSEQUENCER_H_

class MotionSequencer
{
public:
  MotionSequencer();
  virtual ~MotionSequencer();

private: // forbidden default functions
  MotionSequencer& operator = (const MotionSequencer& );  // assignment operator
  MotionSequencer(const MotionSequencer& src);            // copy constructor
};

#endif /* PROD_FISHACTUATOR_SRC_MOTIONSEQUENCER_H_ */
