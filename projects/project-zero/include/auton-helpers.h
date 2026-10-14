// auton-helpers.h
//
// Small, reusable autonomous building blocks.
//
// The MC55-driven DC motors on this robot have NO built-in encoder, so
// the default, always-available moves here are TIME-BASED: "drive at
// this power for this many seconds." That's the standard, no-extra-parts
// way to run autonomous on this hardware -- see config.h's "AUTONOMOUS --
// TIME-BASED MOVES" section for why.
//
// If you add the optional sensors described in config.h's "OPTIONAL
// SENSORS" section (a Rotation Sensor per side for distance, an Inertial
// Sensor for heading), the feedback-based driveStraight()/
// turnToHeading() functions below light up automatically -- no other
// file needs to change.

#pragma once

#include "config.h"

// ----------------------------------------------------------------------------
// PID controller
// ----------------------------------------------------------------------------
// A PID controller does not know anything about motors, inches, or
// degrees -- you feed it "how far am I from the goal right now" (the
// error) every loop, and it hands back "how hard should I push" (the
// output). That separation is what would let the same PID class drive
// both a straight-line move and a turn if you add the sensors needed to
// measure error for each. It's kept here as a ready-to-use building
// block even before you wire anything up.
class PID {
public:
  PID(double kP, double kI, double kD, double integralLimit = 20.0)
      : kP(kP), kI(kI), kD(kD), integralLimit(integralLimit) {
    reset();
  }

  // Call this once right before starting a new move, so leftover error
  // from the previous move doesn't leak into this one.
  void reset(void) {
    integral = 0.0;
    previousError = 0.0;
    firstUpdate = true;
  }

  // Feed in the current error (goal - actual) and get back a motor power
  // suggestion. Does NOT clamp the output -- callers decide the max power.
  double update(double error) {
    // Proportional: bigger error -> bigger push, right now.
    double pTerm = kP * error;

    // Integral: error that has been building up over time (helps erase a
    // small steady-state offset). Clamped so it cannot "wind up" forever
    // and cause a huge overshoot once the robot finally gets moving.
    integral += error;
    if (integral > integralLimit) integral = integralLimit;
    if (integral < -integralLimit) integral = -integralLimit;
    double iTerm = kI * integral;

    // Derivative: how fast the error is changing. Pushes back against
    // fast swings, which is what tames the overshoot a high kP causes.
    // Skipped on the very first update since there is no previous error
    // yet to compare against.
    double derivative = firstUpdate ? 0.0 : (error - previousError);
    double dTerm = kD * derivative;

    previousError = error;
    firstUpdate = false;

    return pTerm + iTerm + dTerm;
  }

private:
  double kP, kI, kD;
  double integral;
  double integralLimit;
  double previousError;
  bool firstUpdate;
};

// ----------------------------------------------------------------------------
// Time-based drive moves (always available -- no extra sensors needed)
// ----------------------------------------------------------------------------

// Drives each side at its own signed percent of max power (-100 to 100,
// see config.h's MAX_DRIVE_VOLTAGE) for the given number of seconds, then
// stops. This one primitive expresses every straight/turn/arc move you
// need without feedback:
//   driveForTime(60, 60, 1.0)   -> straight forward
//   driveForTime(-60, -60, 1.0) -> straight backward
//   driveForTime(50, -50, 0.6)  -> turn in place
//   driveForTime(60, 20, 1.0)   -> a wide curve
// Because there's no distance/heading feedback, the actual result
// depends on battery charge, floor surface, and robot weight -- time
// these empirically on the real robot and expect to re-time them if any
// of that changes.
void driveForTime(double leftPercent, double rightPercent, double seconds);

// Convenience wrapper over driveForTime() for the common "just go
// straight" case.
void driveForwardForTime(double percent, double seconds);

// Convenience wrapper over driveForTime() for the common "just rotate in
// place" case -- left and right sides spin opposite directions at the
// same magnitude.
void turnForTime(double percent, double seconds);

// Immediately stops both drivetrain motors. Useful between autonomous
// moves or as an emergency "stop everything" call.
void stopDrive(void);

// ----------------------------------------------------------------------------
// Feedback-based drive moves (OPTIONAL -- require extra sensors)
// ----------------------------------------------------------------------------
// These only exist (and only need to exist) once you've wired the
// matching sensor and flipped its switch in config.h. If you haven't,
// skip this section entirely -- the time-based moves above are a
// complete, working autonomous toolkit on their own.

#if USE_DRIVE_ROTATION_SENSORS
// Drives forward (positive inches) or backward (negative inches) using a
// PID loop over the two Rotation Sensors' measured distance. Blocks until
// the move finishes or DRIVE_TIMEOUT_SEC elapses, whichever comes first.
void driveStraight(double inches);
#endif

#if USE_INERTIAL_SENSOR
// Rotates in place to face an absolute compass heading (0-360 degrees)
// using a PID loop over the Inertial Sensor's heading reading. Also
// respects a timeout.
void turnToHeading(double headingDegrees);
#endif
