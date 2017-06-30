/*
 * CapSensorDbgCommand.h
 *
 *  Created on: 30.06.2017
 *      Author: nid
 */

#ifndef LIB_CAPSENSOR_CAPSENSORDBGCOMMAND_H_
#define LIB_CAPSENSOR_CAPSENSORDBGCOMMAND_H_

#include <DbgCliCommand.h>

class DbgCli_Topic;
class CapSensor;

//-----------------------------------------------------------------------------

class DbgCli_Cmd_CapSensAct : public DbgCli_Command
{
private:
  CapSensor* m_capSensor;
public:
  DbgCli_Cmd_CapSensAct(DbgCli_Topic* capSensorTopic, CapSensor* capSensor);
  void execute(unsigned int argc, const char** args, unsigned int idxToFirstArgToHandle);
  void printUsage();
};

//-----------------------------------------------------------------------------

class DbgCli_Cmd_CapSensStop : public DbgCli_Command
{
private:
  CapSensor* m_capSensor;
public:
  DbgCli_Cmd_CapSensStop(DbgCli_Topic* capSensorTopic, CapSensor* capSensor);
  void execute(unsigned int argc, const char** args, unsigned int idxToFirstArgToHandle);
  void printUsage();
};

//-----------------------------------------------------------------------------

#endif /* LIB_CAPSENSOR_CAPSENSORDBGCOMMAND_H_ */
