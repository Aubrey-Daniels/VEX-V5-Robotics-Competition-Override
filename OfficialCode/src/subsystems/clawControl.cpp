/*----------------------------------------------------------------------------*/
/*  clawControl.cpp - Claw servo                                              */
/*                                                                            */
/*  CONTROLS                                                                  */
/*    Press L2 -> toggles the claw between two positions                      */
/*                (0 degrees <-> 90 degrees)                                  */
/*                                                                            */
/*  Uses the same "toggle on a new press" method as wristControl.cpp:         */
/*  the claw only changes on the moment L2 goes from released to pressed.     */
/*                                                                            */
/*  NOTE: which angle is "open" and which is "closed" depends on how the      */
/*  claw is mounted. Update the two constants below to match your robot.      */
/*----------------------------------------------------------------------------*/

#include "vex.h"
#include "robot-config.h"
#include "functions.h"

using namespace vex;

// ---- Tunable settings ------------------------------------------------------
static const int CLAW_POSITION_A_DEGREES = 0;   // Claw position at start
static const int CLAW_POSITION_B_DEGREES = 90;  // Claw position after one press
static const int LOOP_DELAY_MS           = 20;  // Time between button checks (ms)

int clawControl() {
  bool clawToggled = false;      // false = position A, true = position B
  bool buttonWasPressed = false; // Was L2 pressed on the previous loop?

  while (1) {
    bool buttonPressed = Controller1.ButtonL2.pressing();

    // New press detected: flip the state and move the servo.
    if (buttonPressed && !buttonWasPressed) {
      clawToggled = !clawToggled;
      claw_servo.setPosition(
          clawToggled ? CLAW_POSITION_B_DEGREES : CLAW_POSITION_A_DEGREES,
          degrees);
    }
    buttonWasPressed = buttonPressed; // Remember for the next loop

    wait(LOOP_DELAY_MS, msec);
  }

  return 0; // Never reached; required because the function returns int
}
