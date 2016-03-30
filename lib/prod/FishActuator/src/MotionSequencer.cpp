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

class MyCmdSeqAdapter : public CmdAdapter
{
private:
  MotionSequencer* m_sequencer;

public:
  MyCmdSeqAdapter(MotionSequencer* sequencer)
  : m_sequencer(sequencer)
  { }

  void moveToAngleAction(int angle)
  {
    m_sequencer->collection()->hal()->setAngle(m_sequencer->fishInMotion()->getFishHwId(), angle);
  }

  void stopAction() { }

private: // forbidden default functions
  MyCmdSeqAdapter& operator = (const MyCmdSeqAdapter& );  // assignment operator
  MyCmdSeqAdapter(const MyCmdSeqAdapter& src);            // copy constructor
};

//-----------------------------------------------------------------------------

CmdMoveToAngle::CmdMoveToAngle(CmdSequence* cmdSeq, int angle)
: CmdHandler(cmdSeq, 200, "CmdMoveToAngle")
, m_angle(angle)
{ }

void CmdMoveToAngle::execute()
{
  if (0 != cmdSequence())
  {
    MyCmdSeqAdapter* adapter = static_cast<MyCmdSeqAdapter*>(cmdSequence()->adapter());
    if (0 != adapter)
    {
      adapter->moveToAngleAction(m_angle);
    }
  }
}

//-----------------------------------------------------------------------------

MotionSequencer::MotionSequencer(FishCollection* collection)
: m_sequence(new CmdSequence(new MyCmdSeqAdapter(this)))
, m_collection(collection)
, m_fishInMotion(0)
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
  delete m_sequence->adapter();
  m_sequence->attachAdapter(0);
  delete m_sequence;
  m_sequence = 0;
}

void MotionSequencer::prepareSequence()
{

  new CmdMoveToAngle(m_sequence, -90);
  new CmdMoveToAngle(m_sequence,   0);
  new CmdMoveToAngle(m_sequence,  90);
  new CmdMoveToAngle(m_sequence,   0);
}

FishCollection* MotionSequencer::collection()
{
  return m_collection;
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

