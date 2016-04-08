/*
 * VelocityControl.cpp
 *
 *  Created on: 30.03.2016
 *      Author: niklausd
 */

#include <VelocityControl.h>
#include <MotionSequencer.h>
#include <Timer.h>
#include <Fish.h>
#include <FishHal.h>
#include <FishCollection.h>
//#include <FishActuator.h>

//-----------------------------------------------------------------------------

class VelocityControlTimerAdapter : public TimerAdapter
{
private:
  VelocityControl* m_verlocityControl;

public:
  VelocityControlTimerAdapter(VelocityControl* verlocityControl)
  : m_verlocityControl(verlocityControl)
  { }

  void timeExpired()
  {
    m_verlocityControl->doAngleControl();
  }

private: // forbidden default functions
  VelocityControlTimerAdapter();
  VelocityControlTimerAdapter& operator = (const VelocityControlTimerAdapter& );  // assignment operator
  VelocityControlTimerAdapter(const VelocityControlTimerAdapter& src);            // copy constructor
};

//-----------------------------------------------------------------------------

VelocityControl::VelocityControl(MotionSequencer* sequencer)
: m_sequencer(sequencer)
, m_ctrlTimer(new Timer(new VelocityControlTimerAdapter(this), Timer::IS_RECURRING))
, m_angle(0)
, m_targetAngle(0)
, m_angleVelocity(1)
, m_ctrlIntervalMillis(20)
{ }

VelocityControl::~VelocityControl()
{
  delete m_ctrlTimer->adapter();
  m_ctrlTimer->attachAdapter(0);
  delete m_ctrlTimer;
  m_ctrlTimer = 0;
}

void VelocityControl::goToTargetAngle(int angle, int angleVelocity)
{
  m_targetAngle = angle;
  m_angleVelocity = angleVelocity;

  // fetch the fish's current angle position
  m_angle = m_sequencer->fishInMotion()->getAngle();

  // let the control process run automatically, start the recurring timer
  m_ctrlTimer->startTimer(m_ctrlIntervalMillis);
}

void VelocityControl::stop()
{
  // stop the control process, cancel the timer
  m_ctrlTimer->cancelTimer();

  if (0 != m_sequencer->fishInMotion())
  {
//    if (0 != m_sequencer->collection()->adapter())
//    {
//      m_sequencer->collection()->adapter()->notifyFishEvent(m_sequencer->fishInMotion()->getFishHwId(), FishNotificationAdapter::EvtFishStopped);
//    }
    // write current angle back into the fish object
    m_sequencer->fishInMotion()->setAngle(m_angle);
  }
}

void VelocityControl::doAngleControl()
{
  int direction = 1;
  int deltaAngle = m_targetAngle - m_angle;
  if (0 > deltaAngle)
  {
    direction = -1;
  }
  else if (0 == deltaAngle)
  {
    direction = 0;
  }
  else
  {
    direction = 1;
  }
  int deltaOfThisIteration = direction * m_angleVelocity;
  if ((deltaAngle * direction) < m_angleVelocity)
  {
    deltaOfThisIteration = deltaAngle;
  }
  m_angle = m_angle + deltaOfThisIteration;
  m_sequencer->collection()->hal()->setAngle(m_sequencer->fishInMotion()->getFishHwId(), m_angle);
  m_sequencer->fishInMotion()->setAngle(m_angle);
  if (0 == deltaAngle)
  {
    // Target angle reached, control process to be stopped
    m_ctrlTimer->cancelTimer();

    // step over to the next command
    m_sequencer->execNextCmd();
  }
}
