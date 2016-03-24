/*
 * FishHal.h
 *
 *  Created on: 24.03.2016
 *      Author: niklausd
 */

#ifndef LIB_PROD_FISHACTUATOR_SRC_FISHHAL_H_
#define LIB_PROD_FISHACTUATOR_SRC_FISHHAL_H_

class Adafruit_PWMServoDriver;
class DbgCli_Command;

class FishHal
{
public:
  FishHal();
  virtual ~FishHal();

  /**
   * Set a particular angle the Servo shall be set to.
   * @param portId Port ID {0..n-1}
   * @param angle Angle to be set {-90 .. 90}
   */
  void setAngle(unsigned int portId, int angle);
private:
  int m_SDA_PIN;
  int m_SCL_PIN;
  Adafruit_PWMServoDriver* m_pwm;
  DbgCli_Command* m_dbgCmd;

private:  // forbidden functions
  FishHal(const FishHal& src);              // copy constructor
  FishHal& operator = (const FishHal& src); // assignment operator
};

#endif /* LIB_PROD_FISHACTUATOR_SRC_FISHHAL_H_ */
