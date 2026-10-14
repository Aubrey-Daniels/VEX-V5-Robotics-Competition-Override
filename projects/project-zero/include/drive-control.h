// drive-control.h
//
// Translates joystick input into drivetrain motor power during the driver
// control period. Supports both TANK and ARCADE drive styles (see
// config.h for the DriveMode concept) and lets the driver swap between them
// live with a controller button.

#pragma once

// Call once from pre_auton() to wire up the drive-mode toggle button.
void driveControlInit(void);

// Call once per loop iteration inside usercontrol(). Reads the controller,
// applies the deadband, applies whichever drive mode is currently active,
// and commands the drivetrain motors.
void driveControlUpdate(void);

// Lets the driver flip between TANK and ARCADE at the field without a code
// change. Wired to a controller button in drive-control.cpp; call this if
// you want to trigger a mode swap from somewhere else (e.g. a different
// button, or an autonomous selector screen).
void toggleDriveMode(void);
