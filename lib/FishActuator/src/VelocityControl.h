/*
 * VelocityControl.h
 *
 *  Created on: 30.03.2016
 *      Author: niklausd
 */

#ifndef PROD_FISHACTUATOR_SRC_VELOCITYCONTROL_H_
#define PROD_FISHACTUATOR_SRC_VELOCITYCONTROL_H_

class Timer;
class MotionSequencer;
class Fish;
class FishHal;

class VelocityControl
{
public:
  VelocityControl(MotionSequencer* sequencer);
  virtual ~VelocityControl();

  void goToTargetAngle(int angle, int angleVelocity);
  void stop();

  void doAngleControl();

private:
  MotionSequencer* m_sequencer;
  Timer* m_ctrlTimer;
  int m_angle;
  int m_targetAngle;
  int m_angleVelocity;   /// [°/interval]
  unsigned long m_ctrlIntervalMillis;

private: // forbidden default functions
  VelocityControl();
  VelocityControl& operator = (const VelocityControl& );  // assignment operator
  VelocityControl(const VelocityControl& src);            // copy constructor
};

#endif /* PROD_FISHACTUATOR_SRC_VELOCITYCONTROL_H_ */
