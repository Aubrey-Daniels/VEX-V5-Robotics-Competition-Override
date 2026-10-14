// robot-config.h
//
// Declares the hardware objects (motors, sensors, controller, brain) that
// exist on the real robot. The actual objects are created once in
// robot-config.cpp; every other file just needs to know they exist via
// these "extern" declarations, so this header stays short and never changes
// just because autonomous or driver-control logic changes.
//
// Port numbers, reversals, and voltage limits live in config.h, not here --
// this file is only "what hardware do we have", not "how is it configured".

#pragma once

#include "vex.h"
#include "config.h"

using namespace vex;

extern brain      Brain;
extern controller Controller1;

// Drivetrain: two 2-wire DC motors, each driven through a VEX Motor
// Controller 55 -- `motor55`, not `motor` (Smart Motors). There is no
// motor_group here on purpose: motor_group is a Smart Motor feature, and
// with exactly one physical motor per side there's nothing to group.
extern motor55 LeftDrive;
extern motor55 RightDrive;

// One 3-wire servo for the practice tests in servo-tests.*. Not part of
// the drivetrain. The port letter lives in config.h.
extern servo TestServo;

// Optional sensors -- only declared/created at all when the matching
// switch in config.h is turned on. See the "OPTIONAL SENSORS" section of
// config.h for what each one is for.
#if USE_INERTIAL_SENSOR
extern inertial Inertial;
#endif

#if USE_DRIVE_ROTATION_SENSORS
extern rotation LeftRotation;
extern rotation RightRotation;
#endif

// Competition control object -- required by the VEX competition switch /
// field control system to call autonomous()/usercontrol() at the right
// time. Declared once in src/main.cpp.
extern competition Competition;

// One-time hardware setup (brake mode, etc). Called from pre_auton() in
// main.cpp.
void robotConfigInit(void);
