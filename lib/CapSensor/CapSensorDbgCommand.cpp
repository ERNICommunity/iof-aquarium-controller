/*
 * CapSensorDbgCommand.cpp
 *
 *  Created on: 30.06.2017
 *      Author: nid
 */

#include <Arduino.h>
#include <DbgTracePort.h>
#include <DbgCliTopic.h>
#include <CapSensor.h>
#include <CapSensorDbgCommand.h>

DbgCli_Cmd_CapSensAct::DbgCli_Cmd_CapSensAct(DbgCli_Topic* capSensorTopic, CapSensor* capSensor)
: DbgCli_Command(capSensorTopic, "act", "Activate touch sensor.")
, m_capSensor(capSensor)
{ }

void DbgCli_Cmd_CapSensAct::execute(unsigned int argc, const char** args, unsigned int idxToFirstArgToHandle)
{
  if (argc - idxToFirstArgToHandle > 0)
  {
    printUsage();
  }
  else
  {
    if ((0 != m_capSensor) && (0 != m_capSensor->adapter()))
    {
      m_capSensor->adapter()->notifyFishActivation();
    }
  }
}

void DbgCli_Cmd_CapSensAct::printUsage()
{
  Serial.println(getHelpText());
  Serial.println("Usage: dbg capsens act");
}

//-----------------------------------------------------------------------------

DbgCli_Cmd_CapSensStop::DbgCli_Cmd_CapSensStop(DbgCli_Topic* capSensorTopic, CapSensor* capSensor)
: DbgCli_Command(capSensorTopic, "stop", "Touch stop touch sensor.")
, m_capSensor(capSensor)
{ }

void DbgCli_Cmd_CapSensStop::execute(unsigned int argc, const char** args, unsigned int idxToFirstArgToHandle)
{
  if (argc - idxToFirstArgToHandle > 0)
  {
    printUsage();
  }
  else
  {
    if ((0 != m_capSensor) && (0 != m_capSensor->adapter()))
    {
      m_capSensor->adapter()->notifyFishStopAll();
    }
  }
}

void DbgCli_Cmd_CapSensStop::printUsage()
{
  Serial.println(getHelpText());
  Serial.println("Usage: dbg capsens stop");
}
