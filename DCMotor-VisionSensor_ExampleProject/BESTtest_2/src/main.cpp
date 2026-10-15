/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       zachb_d                                                   */
/*    Created:      10/8/2026                                                 */
/*    Description:  Example V5 project: motor control + AI Vision sensor,     */
/*                  using the standard VEX competition template.              */
/*                                                                            */
/*  PROJECT LAYOUT                                                            */
/*    include/robot-config.h   hardware declarations (extern)                 */
/*    src/robot-config.cpp     hardware definitions (ports, color signatures) */
/*    include/motor-demo.h     \  motor example                               */
/*    src/motor-demo.cpp       /                                              */
/*    include/vision-demo.h    \  AI Vision example                           */
/*    src/vision-demo.cpp      /                                              */
/*    src/main.cpp             this file: program entry point + competition   */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#include "vex.h"
#include "robot-config.h"
#include "motor-demo.h"
#include "vision-demo.h"

using namespace vex;

// The competition object. It watches the field controller (or the controller's
// competition switch) and automatically calls autonomous() or usercontrol()
// at the right time.
competition Competition;

/*---------------------------------------------------------------------------*/
/*                          Pre-Autonomous Function                          */
/*                                                                           */
/*  Runs once when the program starts, before the match begins. Use it for   */
/*  setup such as resetting sensors or setting starting positions. It must   */
/*  finish (return) or the autonomous/driver code will never start.          */
/*---------------------------------------------------------------------------*/
void pre_auton(void) {
  // Nothing needed for this example. Typical uses: clearing encoders,
  // calibrating an inertial sensor, setting motor brake modes.
}

/*---------------------------------------------------------------------------*/
/*                             Autonomous Task                               */
/*                                                                           */
/*  Runs once at the start of the autonomous period (the robot drives itself */
/*  with no driver input). Put your autonomous routine here.                 */
/*---------------------------------------------------------------------------*/
void autonomous(void) {
  // Empty in this example.
}

/*---------------------------------------------------------------------------*/
/*                            User Control Task                              */
/*                                                                           */
/*  Runs during the driver-controlled period. It repeats in a loop, reading  */
/*  the controller and updating motors each time through.                    */
/*---------------------------------------------------------------------------*/
void usercontrol(void) {
  while (1) {
    // Read joysticks / buttons here and command motors accordingly.
    // (Empty in this example.)

    // Short pause so the loop doesn't run flat-out and waste CPU time.
    wait(20, msec);
  }
}

/*---------------------------------------------------------------------------*/
/*                                  main                                     */
/*                                                                           */
/*  The program starts here. It registers the competition callbacks, runs    */
/*  setup, starts the vision task, then keeps running forever.               */
/*---------------------------------------------------------------------------*/
int main() {
  // Tell the competition object which functions to run for each match phase.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  // Run setup once.
  pre_auton();

  // Start the AI Vision function as a background task. It runs alongside the
  // loop below. (The task keeps running as long as the variable exists.)
  vex::task redDetectTask(visionSensorFunction);

  // main() must never return, or the program ends. This loop demonstrates the
  // motor example: spin motor1 at 12 volts (full power) for 10 seconds, wait
  // 100 ms, then repeat.
  while (true) {
    driveMotor(12);
    wait(100, msec);
  }
}
