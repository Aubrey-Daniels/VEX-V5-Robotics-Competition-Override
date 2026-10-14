/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Project:      VEX V5 Competition Starter                               */
/*    Description:  Beginner-friendly V5 C++ template for the VEX Robotics   */
/*                   VS Code extension (NOT PROS).                           */
/*                                                                            */
/*----------------------------------------------------------------------------*/
//
// This file is intentionally "thin": it only wires the three competition
// callbacks together and calls into the real logic in other files. If
// you're hunting for driver control, PID, or autonomous routines, they
// live in drive-control.*, auton-helpers.*, and autons.* -- not here.
//
// -----------------------------------------------------------------------
// How a VEX competition match actually calls your code
// -----------------------------------------------------------------------
// The V5 competition switch / field controller does NOT run your program
// top-to-bottom like a script. Instead, it calls two functions you
// register ahead of time, at whatever moment the match reaches that
// phase:
//   - autonomous()   is called once, right when the 15-second autonomous
//                     period begins. Your code does NOT control when this
//                     happens -- the field does.
//   - usercontrol()  is called once, right when driver control begins.
//                     It's expected to run forever (an infinite loop)
//                     until the match ends and the Brain cuts power to
//                     the loop's thread.
// pre_auton() is different: it is NOT a competition phase. It is called
// once, immediately, by main() below -- typically while the robot is
// sitting on the field, powered on but still "disabled" (before the match
// has started). It's the right place for one-time setup: calibrating
// sensors, and (in this template) letting the driver pick an autonomous
// routine from the selector in autons.cpp.

#include "vex.h"
#include "robot-config.h"
#include "drive-control.h"
#include "servo-tests.h"
#include "auton-helpers.h"
#include "autons.h"
#include "config.h"

using namespace vex;

// The competition control object. Creating it here (rather than in
// robot-config.cpp) keeps the autonomous()/usercontrol() wiring next to
// the functions it's wiring together.
competition Competition;

void pre_auton(void) {
  robotConfigInit();
  driveControlInit();

#if USE_INERTIAL_SENSOR
  // The inertial sensor (IMU) measures heading using a spinning gyroscope
  // -- like any gyroscope, it needs a brief still "calibration" period
  // right after power-on to find its zero point. Keep the robot
  // perfectly still while this runs, or every later turnToHeading() call
  // will be systematically off. This block only compiles in once you've
  // wired an Inertial Sensor and set USE_INERTIAL_SENSOR to 1 in
  // config.h -- the drivetrain motors (MC55 + DC motors) don't provide
  // any heading feedback on their own.
  Inertial.calibrate();
  while (Inertial.isCalibrating()) {
    wait(25, msec);
  }
#endif

  // Let the driver browse and pick an autonomous routine on the Brain
  // screen. This call blocks, but only until the field enables the
  // robot -- see the comment in autons.cpp for why that's safe.
  autonSelectorRun();
}

void autonomous(void) {
  runSelectedAuton();
}

void usercontrol(void) {
  // Servo buttons are registered here, not in pre_auton(). Up/Down/A are
  // the auton selector's buttons while the robot is still disabled; they
  // become servo controls only once driver control has actually started.
  servoTestsInit();

  while (true) {
    driveControlUpdate();
    servoTestsUpdate();

    // A short, regular pause each loop. Without it, this loop would spin
    // as fast as the CPU allows, burning processor time the rest of the
    // robot's background tasks (sensors, the competition manager, Wi-Fi)
    // also need -- 20ms matches the VEX controller's own update rate, so
    // there's nothing to gain by polling faster than that anyway.
    wait(20, msec);
  }
}

int main() {
  // Register our functions with the competition manager BEFORE calling
  // pre_auton(). This order matters: once registered, the field can call
  // autonomous()/usercontrol() the instant the match state changes, even
  // while pre_auton() (and its selector loop) is still running.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  pre_auton();

  // main() itself must never return while the program is running on the
  // Brain -- this loop just idles, since all of the real work happens in
  // the competition callbacks above.
  while (true) {
    wait(100, msec);
  }
}
