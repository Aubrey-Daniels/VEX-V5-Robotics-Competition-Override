/*----------------------------------------------------------------------------*/
/*  driverControl.cpp - Drivebase (tank drive)                                */
/*                                                                            */
/*  CONTROLS                                                                  */
/*    Left  joystick, vertical (Axis 3) -> left  side of the drivebase        */
/*    Right joystick, vertical (Axis 2) -> right side of the drivebase        */
/*                                                                            */
/*  HOW IT WORKS                                                              */
/*    1. Read each joystick (-100 to +100 percent).                           */
/*    2. Convert percent to motor voltage (-12 V to +12 V).                   */
/*    3. If the voltage is bigger than a small deadband, spin the motor.      */
/*       Otherwise stop it (prevents drift from a not-quite-centered stick).  */
/*    4. Wait 20 ms and repeat.                                               */
/*----------------------------------------------------------------------------*/

#include "vex.h"
#include "robot-config.h"
#include "functions.h"

using namespace vex;

// ---- Tunable settings ------------------------------------------------------
static const double MAX_VOLTAGE   = 12.0; // Full power for a V5 system (volts)
static const double DEADBAND_VOLTS = 0.1; // Ignore joystick input smaller than this
static const int    LOOP_DELAY_MS = 20;   // Time between updates (milliseconds)

// Drives one side of the robot. Spins the motor at the given voltage, or
// stops it if the voltage is inside the deadband.
static void driveSide(vex::motor55 &motor, double volts) {
  // NOTE: fabs() is used (not abs()) because abs() on some compilers
  // truncates decimals to an integer, which would make a 0.5 V input look
  // like 0 V.
  if (fabs(volts) > DEADBAND_VOLTS) {
    motor.spin(fwd, volts, volt);
  } else {
    motor.stop();
  }
}

int driveRobot_tankConfiguration() {

  while (1) {
    // Step 1: read joysticks (-100 to 100 percent)
    double leftPercent  = Controller1.Axis3.position(percent);
    double rightPercent = Controller1.Axis2.position(percent);

    // Step 2: convert percent to volts
    double leftVolts  = (leftPercent  / 100.0) * MAX_VOLTAGE;
    double rightVolts = (rightPercent / 100.0) * MAX_VOLTAGE;

    // Step 3: apply to each side (the right motor is reversed in
    // robot-config.cpp, so positive volts still means "forward")
    driveSide(left_drive_motor,  leftVolts);
    driveSide(right_drive_motor, rightVolts);

    // Step 4: wait so we don't hog the processor
    wait(LOOP_DELAY_MS, msec);
  }

  return 0; // Never reached; required because the function returns int
}
