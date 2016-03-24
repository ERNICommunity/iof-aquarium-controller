// Do not remove the include below
#include "iof_aquarium_controller.h"

#include "FishActuator.h"

#include "Cmd.h"
#include "DbgCliNode.h"
#include "DbgCliCommand.h"
#include "DbgCliTopic.h"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class TestFishNotificationAdapter : public FishNotificationAdapter
{
public:
  void notifyFishEvent(unsigned int fishHwId, FishEvent event)
  {
    Serial.printf("notifyFishEvent(), fishHwId=%d, event=%s\n", fishHwId,
        event == FishNotificationAdapter::EvtFishActivated ? "FishActivated" :
        event == FishNotificationAdapter::EvtFishAdded     ? "FishAdded    " :
        event == FishNotificationAdapter::EvtFishDeleted   ? "FishDeleted  " :
        event == FishNotificationAdapter::EvtFishStopped   ? "FishStopped  " : "UNKNOWN");
  }

  void notifyFishError(unsigned int fishHwId, FishError error)
  {
    Serial.printf("notifyFishError(), fishHwId=%d, error=%s\n", fishHwId,
        error == FishNotificationAdapter::ErrFishAlreadyExists ? "FishAlreadyExists" :
        error == FishNotificationAdapter::ErrFishBusy          ? "FishBusy         " :
        error == FishNotificationAdapter::ErrFishNotFound      ? "FishNotFound     " : "UNKNOWN");
  }
};

//-----------------------------------------------------------------------------
// Debugging
//-----------------------------------------------------------------------------
class DbgCli_Command_AddFish : public DbgCli_Command
{
private:
  FishActuator* m_fishActuator;
public:
  DbgCli_Command_AddFish(FishActuator* fishActuator)
  : DbgCli_Command(new DbgCli_Topic(DbgCli_Node::RootNode(), "fish", "Fish test commands"), "add", "Add Fish; .")
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
// Arduino Cmd I/F
//-----------------------------------------------------------------------------
void dbgCliExecute(int arg_cnt, char **args)
{
  if (0 != DbgCli_Node::RootNode())
  {
    const unsigned int firstArgToHandle = 1;
    DbgCli_Node::RootNode()->execute(static_cast<unsigned int>(arg_cnt), const_cast<const char**>(args), firstArgToHandle);
  }
}

void hello(int arg_cnt, char **args)
{
  Serial.println("Hello world.");
}

//The setup function is called once at startup of the sketch
void setup()
{
  cmdInit(115200); //contains Serial.begin(115200);

  Serial.println();
  Serial.println(F("---------------------------------------------"));
  Serial.println(F("Hello from IoF Aquarium Controller!"));
  Serial.println(F("---------------------------------------------"));
  Serial.println();

  //---------------------------------------------------------------------------
  // Fish Actuator
  //---------------------------------------------------------------------------
  FishActuator* fishActuator = new FishActuator();
  fishActuator->attachAdapter(new TestFishNotificationAdapter());

  //---------------------------------------------------------------------------
  // Debug Cli
  //---------------------------------------------------------------------------
  DbgCli_Node::AssignRootNode(new DbgCli_Topic(0, "dbg", "Aquarium Controller Debug CLI Root Node."));
  //  new DbgCli_Command_FreeRam();
  new DbgCli_Command_AddFish(fishActuator);
  // adding CLI Commands
  cmdAdd("hello", hello);
  cmdAdd("dbg", dbgCliExecute);

  //---------------------------------------------------------------------------
  // Debug Trace
  //---------------------------------------------------------------------------

//  DbgCli_Topic* traceTopic = new DbgCli_Topic(DbgCli_Node::RootNode(), "tr", "Modify debug trace");
//  DbgTrace_Context* traceContext = new DbgTrace_Context(traceTopic);
//  new DbgTrace_Out(DbgTrace_Context::getContext(), "trConOut", new DbgPrint_Console());
}

// The loop function is called in an endless loop
void loop()
{
  yield();
  cmdPoll();
}
