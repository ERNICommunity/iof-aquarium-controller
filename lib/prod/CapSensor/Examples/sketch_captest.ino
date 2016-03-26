#include "CapSensor.h"
#include "Timer.h"



CapSensor cappy(new MyCapSensorAdatper());

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello");
}

void loop() {
  // put your main code here, to run repeatedly:
  //scheduleTimers();

  Serial.println("Running");
}