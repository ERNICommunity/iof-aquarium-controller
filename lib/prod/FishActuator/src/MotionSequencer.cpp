/*
 * MotionSequencer.cpp
 *
 *  Created on: 28.03.2016
 *      Author: nid
 */

#include <MotionSequencer.h>
#include <CmdSequence.h>
#include <CmdAdapter.h>
#include <FishCollection.h>
#include <FishHal.h>
#include <Fish.h>
#include <VelocityControl.h>

//-----------------------------------------------------------------------------

class MyCmdSeqAdapter : public CmdAdapter
{
private:
  MotionSequencer* m_sequencer;

public:
  MyCmdSeqAdapter(MotionSequencer* sequencer)
  : m_sequencer(sequencer)
  { }

  void moveToAngleAction(int angle, int velocity)
  {
    m_sequencer->velocityControl()->goToTargetAngle(angle, velocity);
  }

  void stopAction() { }

private: // forbidden default functions
  MyCmdSeqAdapter();
  MyCmdSeqAdapter& operator = (const MyCmdSeqAdapter& );  // assignment operator
  MyCmdSeqAdapter(const MyCmdSeqAdapter& src);            // copy constructor
};

//-----------------------------------------------------------------------------

CmdMoveToAngle::CmdMoveToAngle(CmdSequence* cmdSeq, unsigned long cmdTimeOutMillis, int angle, int velocity)
: CmdHandler(cmdSeq, cmdTimeOutMillis, "CmdMoveToAngle")
, m_angle(angle)
, m_velocity(velocity)
{ }

void CmdMoveToAngle::execute()
{
  if (0 != cmdSequence())
  {
    MyCmdSeqAdapter* adapter = static_cast<MyCmdSeqAdapter*>(cmdSequence()->adapter());
    if (0 != adapter)
    {
      adapter->moveToAngleAction(m_angle, m_velocity);
    }
  }
}

//-----------------------------------------------------------------------------

const unsigned long MotionSequencer::s_cmdTimeOutMillis = 10000;

MotionSequencer::MotionSequencer(FishCollection* collection, unsigned long cmdTimeOutMillis)
: m_sequence(new CmdSequence(new MyCmdSeqAdapter(this)))
, m_collection(collection)
, m_velocityControl(new VelocityControl(this))
, m_fishInMotion(0)
, m_cmdTimeOutMillis(cmdTimeOutMillis)
{
  FishHal* hal = m_collection->hal();
  if (0 != hal)
  {
    for (unsigned int i = 0; i < 15; i++)
    {
      hal->setAngle(0, i);
    }
  }
  prepareSequence();
}

MotionSequencer::~MotionSequencer()
{
  delete m_velocityControl;
  m_velocityControl = 0;
  delete m_sequence->adapter();
  m_sequence->attachAdapter(0);
  delete m_sequence;
  m_sequence = 0;
}

void MotionSequencer::prepareSequence()
{
  int velocity = 2;
  unsigned long pauseTimeMillis = 500;
  new CmdMoveToAngle(m_sequence, m_cmdTimeOutMillis, -90, velocity);
  new CmdStop(m_sequence, pauseTimeMillis);
  new CmdMoveToAngle(m_sequence, m_cmdTimeOutMillis,  90, velocity);
  new CmdStop(m_sequence, pauseTimeMillis);
  new CmdMoveToAngle(m_sequence, m_cmdTimeOutMillis,   0, velocity);
}

FishCollection* MotionSequencer::collection()
{
  return m_collection;
}

VelocityControl* MotionSequencer::velocityControl()
{
  return m_velocityControl;
}

void MotionSequencer::startSequence(Fish* fishInMotion)
{
  m_fishInMotion = fishInMotion;
  m_sequence->start();
}

Fish* MotionSequencer::fishInMotion()
{
  return m_fishInMotion;
}

bool MotionSequencer::isRunning()
{
  return m_sequence->isRunning();
}

void MotionSequencer::execNextCmd()
{
  m_sequence->execNextCmd();
}

