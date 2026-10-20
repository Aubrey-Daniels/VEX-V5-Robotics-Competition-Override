/*----------------------------------------------------------------------------*/
/*  cascadeLiftControl.cpp - Cascade lift                                     */
/*                                                                            */
/*  CONTROLS                                                                  */
/*    Hold R1 -> lift goes UP   (full power)                                  */
/*    Hold R2 -> lift goes DOWN (full power)                                  */
/*    Release -> lift stops                                                   */
/*                                                                            */
/*  This is "hold to move" control: the lift only moves while a button is     */
/*  held. If both buttons are held, R1 (up) wins.                             */
/*----------------------------------------------------------------------------*/

#include "vex.h"
#include "robot-config.h"
#include "functions.h"

using namespace vex;

// ---- Tunable settings ------------------------------------------------------
static const double LIFT_VOLTAGE  = 12.0; // Lift power in volts (max 12)
static const int    LOOP_DELAY_MS = 20;   // Time between button checks (ms)

int cascadeLiftControl() {
  while (1) {
    if (Controller1.ButtonR1.pressing()) {
      cascade_lift_motor.spin(forward, LIFT_VOLTAGE, volt);  // Up
    }
    else if (Controller1.ButtonR2.pressing()) {
      cascade_lift_motor.spin(reverse, LIFT_VOLTAGE, volt);  // Down
    }
    else {
      cascade_lift_motor.stop();                             // Neither pressed
    }

    wait(LOOP_DELAY_MS, msec);
  }

  return 0; // Never reached; required because the function returns int
}
