// servo-tests.h
//
// Practice moves for the one 3-wire servo declared in robot-config.h.
//
// A servo holds an angle you ask for. The drive motors do the opposite:
// they keep spinning at a voltage until you command them to stop, and
// they have no angle to hold. These tests never command LeftDrive or
// RightDrive -- driver control keeps doing that on its own.

#pragma once

// Call once at the start of usercontrol(), after the auton selector has
// finished. While the robot is disabled, ButtonUp / ButtonDown / ButtonA
// belong to the auton selector. This is what hands those buttons to the
// servo once the driver is actually driving.
void servoTestsInit(void);

// Call once per driver-control loop, next to driveControlUpdate(). When
// a sweep is running, this walks the commanded angle a small step. It
// does not pause the loop, so the joysticks still drive the robot.
void servoTestsUpdate(void);
