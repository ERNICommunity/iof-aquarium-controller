/*
 * FishHal.cpp
 *
 *  Created on: 24.03.2016
 *      Author: niklausd
 */

#include <FishHal.h>
#include <Adafruit_PWMServoDriver.h>
#include <Arduino.h>
#include <DbgCliCommand.h>
#include <DbgCliNode.h>
#include <DbgCliTopic.h>

class DbgCli_Command_FishAngle : public DbgCli_Command
{
public:
  DbgCli_Command_FishAngle(FishHal* hal, DbgCli_Topic* dbgCliTopic)
  : DbgCli_Command(dbgCliTopic, "ang", "Set Angle.")
  , m_hal(hal)
  { }

  virtual ~DbgCli_Command_FishAngle() { }

  void execute(unsigned int argc, const char** args, unsigned int idxToFirstArgToHandle)
  {
    if (argc < 5)
    {
      printUsage();
    }
    else
    {
      unsigned int portId = atoi(args[idxToFirstArgToHandle]);
      int angle = atoi(args[idxToFirstArgToHandle+1]);
      Serial.print("DbgCli_Command_FishAngle, setAngle(");
      Serial.print(portId);
      Serial.print(", ");
      Serial.print(angle);
      Serial.println(")");
      m_hal->setAngle(portId, angle);
    }
  }

private:
  void printUsage()
  {
    Serial.println("dbg hal ang - usage: <portId> {1..992} <angle> {-90 .. 90}");
  }

private:
  FishHal* m_hal;

private: // forbidden default functions
  DbgCli_Command_FishAngle();
  DbgCli_Command_FishAngle& operator = (const DbgCli_Command_FishAngle& src);  // assignment operator
  DbgCli_Command_FishAngle(const DbgCli_Command_FishAngle& src);               // copy constructor
};

#define SERVOMIN  160 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  555 // this is the 'maximum' pulse length count (out of 4096)

FishHal::FishHal()
: m_pwm(new Adafruit_PWMServoDriver())
, m_dbgCliTopic(new DbgCli_Topic(DbgCli_Node::RootNode(), "hal", "Fish HAL test commands."))
{
  m_pwm->begin();
  m_pwm->setPWMFreq(60);  // Analog servos run at ~60 Hz updates

  new DbgCli_Command_FishAngle(this, m_dbgCliTopic);
}

FishHal::~FishHal()
{ }

void FishHal::setAngle(unsigned int portId, int angle)
{
  int angleBiased = angle + 90;
  int pulselen = map(angleBiased, 0, 180, SERVOMIN, SERVOMAX);
  m_pwm->setPWM(portId-1, 0, pulselen);
}

