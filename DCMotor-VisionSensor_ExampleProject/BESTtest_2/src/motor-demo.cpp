/*----------------------------------------------------------------------------*/
/*  motor-demo.cpp - Example: spinning a motor with voltage                   */
/*----------------------------------------------------------------------------*/

#include "vex.h"
#include "motor-demo.h"
#include "robot-config.h"  // gives us access to motor1

// Spin motor1 forward at `voltage` volts for 10 seconds, then stop it.
// Motor voltage ranges from -12 to +12 volts (12 = full power).
//
// NOTE: wait() BLOCKS, meaning the code that called this function does
// nothing else for the 10 seconds it takes to finish.
void driveMotor(int voltage) {
  // Start the motor. spin() returns immediately; the motor keeps running.
  motor1.spin(vex::directionType::fwd, voltage, vex::voltageUnits::volt);

  // Let it run for 10 seconds.
  wait(10, vex::seconds);

  // Stop by commanding 0 volts.
  motor1.spin(vex::directionType::fwd, 0, vex::voltageUnits::volt);
}
