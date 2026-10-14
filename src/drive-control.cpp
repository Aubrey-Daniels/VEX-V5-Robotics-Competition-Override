// drive-control.cpp
//
// Driver control (teleop) joystick-to-motor logic for a 2-motor DC
// drivetrain driven through VEX Motor Controller 55 units. Two "drive
// modes" are supported -- TANK and ARCADE -- see config.h for what those
// mean and how to pick a default. The driver can also flip modes live on
// the field.
//
// Why voltage, not percent: `motor55` has no built-in speed control loop
// (that's a Smart Motor feature), so there's no "spin at X% speed"
// command for it the way there is for `motor`. What you *can* command is
// a voltage, and a plain DC motor's speed is roughly proportional to the
// voltage you give it -- so "percent power" here just means "what
// percentage of our max allowed voltage to send," converted to volts
// right before each spin() call.

#include "vex.h"
#include "robot-config.h"
#include "drive-control.h"
#include "config.h"

using namespace vex;

// Starts at the project default but can change at runtime via the button
// below, so it is a plain variable (not a const) local to this file.
static DriveMode currentDriveMode = DEFAULT_DRIVE_MODE;

// A joystick that is "let go" rarely reads exactly 0 -- small mechanical
// drift means it might report +/-2 or +/-3 percent. Treating anything
// inside the deadband as zero stops the robot from creeping when no one is
// touching the sticks.
static int applyDeadband(int value, int deadband) {
  if (value > -deadband && value < deadband) {
    return 0;
  }
  return value;
}

static double clampPercent(double value, double maxMagnitude) {
  if (value > maxMagnitude) return maxMagnitude;
  if (value < -maxMagnitude) return -maxMagnitude;
  return value;
}

// Converts a signed "percent of max power" value (-100 to 100) into the
// signed voltage motor55::spin() expects. A negative voltage just spins
// the motor the opposite way, so callers can pass a signed percent
// straight through without picking a direction separately.
static double percentToVoltage(double percent) {
  double clamped = clampPercent(percent, 100.0);
  return (clamped / 100.0) * MAX_DRIVE_VOLTAGE;
}

static void driveTank(void) {
  int leftPercent  = applyDeadband(Controller1.Axis3.position(), JOYSTICK_DEADBAND);
  int rightPercent = applyDeadband(Controller1.Axis2.position(), JOYSTICK_DEADBAND);

  LeftDrive.spin(forward, percentToVoltage(leftPercent), volt);
  RightDrive.spin(forward, percentToVoltage(rightPercent), volt);
}

static void driveArcade(void) {
  // Axis3 (left stick, up/down) = how fast to drive forward/backward.
  // Axis1 (right stick, left/right) = how hard to turn.
  int throttle = applyDeadband(Controller1.Axis3.position(), JOYSTICK_DEADBAND);
  int turn     = applyDeadband(Controller1.Axis1.position(), JOYSTICK_DEADBAND);

  // Mixing: add the turn component to one side and subtract it from the
  // other. This is the standard "arcade drive" mix -- it is what lets a
  // single stick's left/right motion curve the robot instead of needing a
  // second stick dedicated to each side. Clamp afterward since
  // throttle+turn can exceed +/-100.
  double leftPercent  = clampPercent(throttle + turn, 100.0);
  double rightPercent = clampPercent(throttle - turn, 100.0);

  LeftDrive.spin(forward, percentToVoltage(leftPercent), volt);
  RightDrive.spin(forward, percentToVoltage(rightPercent), volt);
}

void toggleDriveMode(void) {
  currentDriveMode = (currentDriveMode == DriveMode::TANK)
                          ? DriveMode::ARCADE
                          : DriveMode::TANK;

  Brain.Screen.setCursor(1, 1);
  Brain.Screen.clearLine();
  Brain.Screen.print("Drive mode: %s",
                      currentDriveMode == DriveMode::TANK ? "TANK" : "ARCADE");
}

void driveControlInit(void) {
  // ButtonX toggles drive style; pick whichever button your drivers find
  // easiest to remember. This is intentionally a single line to change.
  Controller1.ButtonX.pressed(toggleDriveMode);
}

void driveControlUpdate(void) {
  if (currentDriveMode == DriveMode::TANK) {
    driveTank();
  } else {
    driveArcade();
  }
}
