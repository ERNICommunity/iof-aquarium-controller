/*
 * MotionSequencer.h
 *
 *  Created on: 28.03.2016
 *      Author: nid
 */

#ifndef PROD_FISHACTUATOR_SRC_MOTIONSEQUENCER_H_
#define PROD_FISHACTUATOR_SRC_MOTIONSEQUENCER_H_

#include <CmdHandler.h>

class CmdSequence;
class FishCollection;
class Fish;

//-----------------------------------------------------------------------------

class CmdMoveToAngle : public CmdHandler
{
public:
  CmdMoveToAngle(CmdSequence* cmdSeq, int angle);
  virtual ~CmdMoveToAngle() { }
  virtual void execute();

private:
  int m_angle;


private: // forbidden default functions
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
  MotionSequencer(FishCollection* collection);
  virtual ~MotionSequencer();

  void startSequence(Fish* fishInMotion);

  FishCollection* collection();

  Fish* fishInMotion();

  bool isRunning();

protected:
  void prepareSequence();

private:
  CmdSequence* m_sequence;
  FishCollection* m_collection;
  Fish* m_fishInMotion;

private: // forbidden default functions
  MotionSequencer& operator = (const MotionSequencer& );  // assignment operator
  MotionSequencer(const MotionSequencer& src);            // copy constructor
};

#endif /* PROD_FISHACTUATOR_SRC_MOTIONSEQUENCER_H_ */
