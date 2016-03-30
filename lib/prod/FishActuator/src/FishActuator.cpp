/*
 * FishActuator.cpp
 *
 *  Created on: 21.03.2016
 *      Author: niklausd
 */

#include <FishActuator.h>
#include <FishCollection.h>
#include <DbgCliTopic.h>
#include <DbgCliCommand.h>

#include <Arduino.h>

//-----------------------------------------------------------------------------
// Debugging
//-----------------------------------------------------------------------------
class DbgCli_Command_AddFish : public DbgCli_Command
{
private:
  FishActuator* m_fishActuator;
public:
  DbgCli_Command_AddFish(FishActuator* fishActuator, DbgCli_Topic* dbgCliTopic)
  : DbgCli_Command(dbgCliTopic, "add", "Add Fish")
  , m_fishActuator(fishActuator)
  { }

  void printUsage()
  {
    Serial.println("dbg fish add - usage: {0..15}");
  }

  void execute(unsigned int argc, const char** args, unsigned int idxToFirstArgToHandle)
  {
    if (argc < 4)
    {
      printUsage();
    }
    else
    {
      unsigned int hwId = atoi(args[idxToFirstArgToHandle]);
      m_fishActuator->addFishAtHwId(hwId);
    }
  }
};

//-----------------------------------------------------------------------------

class DbgCli_Command_DelFish: public DbgCli_Command
{
private:
  FishActuator* m_fishActuator;
public:
  DbgCli_Command_DelFish(FishActuator* fishActuator, DbgCli_Topic* dbgCliTopic)
  : DbgCli_Command(dbgCliTopic, "del", "Delete Fish")
  , m_fishActuator(fishActuator)
  { }

  void printUsage()
  {
    Serial.println("dbg fish add - usage: {0..15}");
  }

  void execute(unsigned int argc, const char** args, unsigned int idxToFirstArgToHandle)
  {
    if (argc < 4)
    {
      printUsage();
    }
    else
    {
      unsigned int hwId = atoi(args[idxToFirstArgToHandle]);
      m_fishActuator->delFishAtHwId(hwId);
    }
  }
};

//-----------------------------------------------------------------------------

FishActuator::FishActuator()
: m_fishCollection(new FishCollection())
, m_dbgCliTopic(new DbgCli_Topic(DbgCli_Node::RootNode(), "fish", "Fish test commands."))
{
  new DbgCli_Command_AddFish(this, m_dbgCliTopic);
  new DbgCli_Command_DelFish(this, m_dbgCliTopic);
}

FishActuator::~FishActuator()
{
  delete m_dbgCliTopic;
  m_dbgCliTopic = 0;
  delete m_fishCollection;
  m_fishCollection = 0;
}

void FishActuator::attachAdapter(FishNotificationAdapter* adapter)
{
  if (0 != m_fishCollection)
  {
    m_fishCollection->attachAdapter(adapter);
  }
}

void FishActuator::addFishAtHwId(unsigned int fishHwId)
{
  if (0 != m_fishCollection)
  {
//    Serial.printf("FishActuator::addFishAtHwId(): Add Fish with HW ID=%d\n", fishHwId);
    m_fishCollection->addFishAtHwId(fishHwId);
  }
}

void FishActuator::delFishAtHwId(unsigned int fishHwId)
{
  if (0 != m_fishCollection)
  {
    m_fishCollection->addFishAtHwId(fishHwId);
  }
}

void FishActuator::activateFish(unsigned int fishHwId)
{
  if (0 != m_fishCollection)
  {
    m_fishCollection->activateFish(fishHwId);
  }
}

DbgCli_Topic* FishActuator::dbgCliTopic()
{
  return m_dbgCliTopic;
}
