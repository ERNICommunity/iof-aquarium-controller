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

class HalDbgCmd_Angle : public DbgCli_Command
{
public:
  HalDbgCmd_Angle(FishHal* hal)
  : DbgCli_Command(new DbgCli_Topic(DbgCli_Node::RootNode(), "hal", "HW Abstraction Layer"), "ang", "Set Angle.")
  , m_hal(hal)
  { }

  virtual ~HalDbgCmd_Angle() { }

  void execute(unsigned int argc, const char** args, unsigned int idxToFirstArgToHandle)
  {
    if (argc < 2)
    {
      printUsage();
    }
    else
    {
      unsigned int portId = atoi(args[idxToFirstArgToHandle]);
      int angle = atoi(args[idxToFirstArgToHandle+1]);
      m_hal->setAngle(portId, angle);
    }
  }

private:
  void printUsage()
  {
    Serial.println("dbg hal - usage: <portId> {0..15} <angle> {-90 .. 90}");
  }

private:
  FishHal* m_hal;

private: // forbidden default functions
  HalDbgCmd_Angle& operator = (const HalDbgCmd_Angle& src);  // assignment operator
  HalDbgCmd_Angle(const HalDbgCmd_Angle& src);               // copy constructor
};

#define SERVOMIN  160 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  570 // this is the 'maximum' pulse length count (out of 4096)

FishHal::FishHal()
: m_pwm(new Adafruit_PWMServoDriver())
, m_dbgCmd(new HalDbgCmd_Angle(this))
{
  m_pwm->begin();
  m_pwm->setPWMFreq(60);  // Analog servos run at ~60 Hz updates
}

FishHal::~FishHal()
{ }

void FishHal::setAngle(unsigned int portId, int angle)
{
  int angleBiased = angle + 90;
  int pulselen = map(angleBiased, 0, 180, SERVOMIN, SERVOMAX);
  m_pwm->setPWM(portId, 0, pulselen);
}

