// auton-helpers.cpp
//
// Implementation of the time-based drive primitives declared in
// auton-helpers.h, plus the optional sensor-feedback moves that only
// compile in once you enable them in config.h.

#include "vex.h"
#include "robot-config.h"
#include "auton-helpers.h"
#include "config.h"
#include <cmath>

using namespace vex;

static double clampPercent(double value, double maxMagnitude) {
  if (value > maxMagnitude) return maxMagnitude;
  if (value < -maxMagnitude) return -maxMagnitude;
  return value;
}

// Same percent-of-max-voltage conversion used in drive-control.cpp -- see
// the comment there for why motor55 is commanded in volts, not percent.
static double percentToVoltage(double percent) {
  double clamped = clampPercent(percent, 100.0);
  return (clamped / 100.0) * MAX_DRIVE_VOLTAGE;
}

void stopDrive(void) {
  LeftDrive.stop();
  RightDrive.stop();
}

void driveForTime(double leftPercent, double rightPercent, double seconds) {
  LeftDrive.spin(forward, percentToVoltage(leftPercent), volt);
  RightDrive.spin(forward, percentToVoltage(rightPercent), volt);

  wait(seconds, sec);

  stopDrive();
}

void driveForwardForTime(double percent, double seconds) {
  driveForTime(percent, percent, seconds);
}

void turnForTime(double percent, double seconds) {
  // Turning in place: one side spins forward, the other spins backward
  // by the same amount, so the robot rotates around its own center
  // instead of driving in an arc.
  driveForTime(percent, -percent, seconds);
}

// ----------------------------------------------------------------------------
// Optional: distance feedback via Rotation Sensors
// ----------------------------------------------------------------------------
#if USE_DRIVE_ROTATION_SENSORS

// How far the wheel travels for one full revolution of the Rotation
// Sensor's shaft. Divided by ROTATION_SENSOR_GEAR_RATIO because that many
// *sensor* revolutions make up one *wheel* revolution (see config.h).
static double inchesPerSensorRev(void) {
  double wheelCircumference = M_PI * WHEEL_DIAMETER_IN;
  return wheelCircumference / ROTATION_SENSOR_GEAR_RATIO;
}

static double getDriveInches(void) {
  double leftInches  = LeftRotation.position(rotationUnits::rev)  * inchesPerSensorRev();
  double rightInches = RightRotation.position(rotationUnits::rev) * inchesPerSensorRev();
  return (leftInches + rightInches) / 2.0;
}

void driveStraight(double inches) {
  PID drivePID(DRIVE_KP, DRIVE_KI, DRIVE_KD);

  LeftRotation.setPosition(0, degrees);
  RightRotation.setPosition(0, degrees);

  timer moveTimer;

  while (true) {
    double error = inches - getDriveInches();

    if (std::fabs(error) < DRIVE_TOLERANCE_IN) break;
    if (moveTimer.time(timeUnits::sec) > DRIVE_TIMEOUT_SEC) break;

    double outputPercent = clampPercent(drivePID.update(error), DRIVE_MAX_PERCENT);
    double voltageOut = percentToVoltage(outputPercent);

    LeftDrive.spin(forward, voltageOut, volt);
    RightDrive.spin(forward, voltageOut, volt);

    wait(20, msec);
  }

  stopDrive();
}

#endif // USE_DRIVE_ROTATION_SENSORS

// ----------------------------------------------------------------------------
// Optional: heading feedback via an Inertial Sensor
// ----------------------------------------------------------------------------
#if USE_INERTIAL_SENSOR

// Keeps a heading error in the range (-180, 180] -- without this, asking
// the robot to turn from a heading of 350 degrees to 10 degrees would
// compute a 340-degree error and spin the long way around instead of just
// turning 20 degrees through zero.
static double normalizeAngleError(double error) {
  while (error > 180.0)  error -= 360.0;
  while (error < -180.0) error += 360.0;
  return error;
}

void turnToHeading(double headingDegrees) {
  PID turnPID(TURN_KP, TURN_KI, TURN_KD);

  timer moveTimer;

  while (true) {
    double error = normalizeAngleError(headingDegrees - Inertial.heading(degrees));

    if (std::fabs(error) < TURN_TOLERANCE_DEG) break;
    if (moveTimer.time(timeUnits::sec) > TURN_TIMEOUT_SEC) break;

    double outputPercent = clampPercent(turnPID.update(error), TURN_MAX_PERCENT);
    double voltageOut = percentToVoltage(outputPercent);

    LeftDrive.spin(forward, voltageOut, volt);
    RightDrive.spin(forward, -voltageOut, volt);

    wait(20, msec);
  }

  stopDrive();
}

#endif // USE_INERTIAL_SENSOR
