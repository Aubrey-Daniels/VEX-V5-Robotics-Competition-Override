// autons.cpp
//
// Example autonomous routines plus a brain-screen selector that lets a
// driver choose one before the match starts.
//
// The default examples are all TIME-BASED ("drive at X power for Y
// seconds") because the MC55-driven DC motors on this robot have no
// built-in encoder -- see config.h for the full explanation. Time your
// own moves on the real robot with a stopwatch: run a candidate
// driveForwardForTime()/turnForTime() call, measure what actually
// happened, and adjust the seconds (or AUTON_DRIVE_POWER_PERCENT in
// config.h) until it's close enough for your field.

#include "vex.h"
#include "robot-config.h"
#include "auton-helpers.h"
#include "autons.h"
#include "config.h"

using namespace vex;

// ----------------------------------------------------------------------------
// Example autonomous routines
// ----------------------------------------------------------------------------

// A safe default: physically does nothing. Having this as an explicit,
// selectable option matters -- if your "real" auton isn't ready yet, or
// your team just wants to guarantee the robot stays still and doesn't
// collide with anything, you can select this instead of risking a half-
// finished routine during a real match.
static void autonDoNothing(void) {
  // Intentionally empty.
}

// The simplest possible "real" autonomous: drive forward at a fixed
// power for a fixed time, long enough to leave the starting tile. Time
// this on your own robot/floor -- 1.5 seconds at 70% power is a
// starting guess, not a measured value for any particular robot.
static void autonDriveForward(void) {
  driveForwardForTime(AUTON_DRIVE_POWER_PERCENT, 1.5); // seconds
}

// Demonstrates chaining driveForwardForTime() and turnForTime() to trace
// an approximate square: drive a side, turn ~90 degrees, repeat four
// times. "Approximate" is the key word -- with no distance or heading
// feedback, small errors in each leg add up over the full loop. This is
// a good routine for *noticing* that drift, and a good argument for
// adding the optional sensors in config.h once your autonomous needs to
// be more precise than this.
static void autonSquarePath(void) {
  for (int side = 0; side < 4; side++) {
    driveForwardForTime(AUTON_DRIVE_POWER_PERCENT, 1.0);
    turnForTime(AUTON_DRIVE_POWER_PERCENT, 0.5); // re-time for a ~90 degree turn on your robot
  }
}

// Only compiled in once you've wired an Inertial Sensor and set
// USE_INERTIAL_SENSOR to 1 in config.h. Shows the same square path idea,
// but with the turns corrected by heading feedback instead of guessed
// timing -- the forward legs are still time-based, since that still
// needs the Rotation Sensors (USE_DRIVE_ROTATION_SENSORS) to fix.
#if USE_INERTIAL_SENSOR
static void autonSquarePathHeadingCorrected(void) {
  for (int side = 0; side < 4; side++) {
    driveForwardForTime(AUTON_DRIVE_POWER_PERCENT, 1.0);
    turnToHeading((side + 1) * 90.0);
  }
}
#endif

// ----------------------------------------------------------------------------
// Selector
// ----------------------------------------------------------------------------
// A simple, explicit list -- no reflection or magic. To add an auton:
//   1. Write `static void myAuton(void) { ... }` above.
//   2. Add { "My Auton Name", myAuton } to this array.
struct AutonEntry {
  const char *name;
  void (*run)(void);
};

static const AutonEntry autonList[] = {
    {"Do Nothing", autonDoNothing},
    {"Drive Forward", autonDriveForward},
    {"Square Path (timed)", autonSquarePath},
#if USE_INERTIAL_SENSOR
    {"Square Path (IMU turns)", autonSquarePathHeadingCorrected},
#endif
};
static const int AUTON_COUNT = sizeof(autonList) / sizeof(autonList[0]);

static int selectedAutonIndex = 0;

static void drawSelectorScreen(void) {
  Brain.Screen.clearScreen();
  Brain.Screen.setCursor(1, 1);
  Brain.Screen.print("Select auton: Up/Down, A to confirm");
  Brain.Screen.setCursor(3, 1);
  Brain.Screen.print("-> %s", autonList[selectedAutonIndex].name);
}

void autonSelectorRun(void) {
  drawSelectorScreen();

  // Track the previous frame's button state so we react once per press
  // ("rising edge") instead of scrolling rapidly while a button is held.
  bool upWasPressed = false;
  bool downWasPressed = false;

  // Keep letting the driver browse choices right up until the field
  // actually enables the robot -- that way selection never costs match
  // time, and there's no risk of this loop "eating" the start of a match.
  while (!Competition.isEnabled()) {
    bool upPressed = Controller1.ButtonUp.pressing();
    bool downPressed = Controller1.ButtonDown.pressing();

    if (upPressed && !upWasPressed) {
      selectedAutonIndex = (selectedAutonIndex + 1) % AUTON_COUNT;
      drawSelectorScreen();
    } else if (downPressed && !downWasPressed) {
      selectedAutonIndex = (selectedAutonIndex - 1 + AUTON_COUNT) % AUTON_COUNT;
      drawSelectorScreen();
    }

    upWasPressed = upPressed;
    downWasPressed = downPressed;

    // ButtonA is an explicit "I'm happy with this choice" confirmation.
    // It doesn't do anything functionally different from just waiting
    // (the selection already took effect immediately above), but it lets
    // the driver lock in a choice and stop fiddling with the screen.
    if (Controller1.ButtonA.pressing()) {
      break;
    }

    wait(20, msec);
  }
}

void runSelectedAuton(void) {
  autonList[selectedAutonIndex].run();
}
