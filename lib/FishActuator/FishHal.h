/*
 * FishHal.h
 *
 *  Created on: 24.03.2016
 *      Author: niklausd
 */

#ifndef LIB_PROD_FISHACTUATOR_SRC_FISHHAL_H_
#define LIB_PROD_FISHACTUATOR_SRC_FISHHAL_H_

class Adafruit_PWMServoDriver;
class DbgCli_Topic;

class FishHal
{
public:
  FishHal();
  virtual ~FishHal();

  /**
   * Set a particular angle the Servo shall be set to.
   * @param portId Port ID {1..n} n = 992
   * @param angle Angle to be set {-90 .. 90}
   */
  void setAngle(unsigned int portId, int angle);

private:
  Adafruit_PWMServoDriver* m_pwm;
  DbgCli_Topic* m_dbgCliTopic;

private:  // forbidden functions
  FishHal(const FishHal& src);              // copy constructor
  FishHal& operator = (const FishHal& src); // assignment operator
};

#endif /* LIB_PROD_FISHACTUATOR_SRC_FISHHAL_H_ */
