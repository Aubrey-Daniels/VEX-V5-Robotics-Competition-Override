/*----------------------------------------------------------------------------*/
/*  wristControl.cpp - Wrist servo                                            */
/*                                                                            */
/*  CONTROLS                                                                  */
/*    Press L1 -> toggles the wrist between two positions                     */
/*                (0 degrees <-> 90 degrees)                                  */
/*                                                                            */
/*  HOW THE TOGGLE WORKS                                                      */
/*    We remember whether L1 was pressed on the previous loop. The wrist      */
/*    only moves on the moment the button goes from "not pressed" to          */
/*    "pressed" (a rising edge). Holding the button down does NOT keep        */
/*    flipping the wrist back and forth.                                      */
/*----------------------------------------------------------------------------*/

#include "vex.h"
#include "robot-config.h"
#include "functions.h"

using namespace vex;

// ---- Tunable settings ------------------------------------------------------
static const int WRIST_DOWN_DEGREES = 0;   // Wrist position at start
static const int WRIST_UP_DEGREES   = 90;  // Wrist position after one press
static const int LOOP_DELAY_MS      = 20;  // Time between button checks (ms)

int wristControl() {
  bool wristUp = false;        // Which position the wrist is currently in
  bool previousButton = false; // Was L1 pressed on the previous loop?

  while (1) {
    bool buttonPressed = Controller1.ButtonL1.pressing();

    // New press detected: flip the state and move the servo.
    if (buttonPressed && !previousButton) {
      wristUp = !wristUp;
      wrist_servo.setPosition(wristUp ? WRIST_UP_DEGREES : WRIST_DOWN_DEGREES,
                              degrees);
    }

    previousButton = buttonPressed; // Remember for the next loop

    wait(LOOP_DELAY_MS, msec);
  }

  return 0; // Never reached; required because the function returns int
}
