/*
 * MotionSequencer.h
 *
 *  Created on: 28.03.2016
 *      Author: nid
 */

#ifndef PROD_FISHACTUATOR_SRC_MOTIONSEQUENCER_H_
#define PROD_FISHACTUATOR_SRC_MOTIONSEQUENCER_H_

#include <Cmd.h>

class CmdSequence;
class FishCollection;
class Fish;
class VelocityControl;

//-----------------------------------------------------------------------------

class CmdMoveToAngle : public Cmd
{
public:
  CmdMoveToAngle(CmdSequence* cmdSeq, unsigned long cmdTimeOutMillis, int angle, int velocity);
  virtual ~CmdMoveToAngle() { }
  virtual void execute();

private:
  int m_angle;
  int m_velocity;

private: // forbidden default functions
  CmdMoveToAngle();
  CmdMoveToAngle& operator = (const CmdMoveToAngle& );  // assignment operator
  CmdMoveToAngle(const CmdMoveToAngle& src);            // copy constructor
};

//-----------------------------------------------------------------------------

class MotionSequencer
{
public:
  /**
   * Constructor.
   * @param collection Pointer to FishCollection object.
   */
  MotionSequencer(FishCollection* collection, unsigned long cmdTimeOutMillis = s_cmdTimeOutMillis);
  virtual ~MotionSequencer();

  FishCollection* collection();
  VelocityControl* velocityControl();

  void startSequence(Fish* fishInMotion);
  void stop();
  Fish* fishInMotion();
  bool isRunning();
  void execNextCmd();

protected:
  void prepareSequence();

private:
  CmdSequence* m_sequence;
  FishCollection* m_collection;
  VelocityControl* m_velocityControl;
  Fish* m_fishInMotion;
  unsigned long m_cmdTimeOutMillis;
  const static unsigned long s_cmdTimeOutMillis;

private: // forbidden default functions
  MotionSequencer& operator = (const MotionSequencer& );  // assignment operator
  MotionSequencer(const MotionSequencer& src);            // copy constructor
};

#endif /* PROD_FISHACTUATOR_SRC_MOTIONSEQUENCER_H_ */
