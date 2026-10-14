// autons.h
//
// Example autonomous routines plus a tiny selector so you can pick which
// one runs without re-flashing code between matches.
//
// Add your own auton by: writing a new `void myAuton(void)` function in
// autons.cpp, then adding its name + function pointer to the `autonList`
// array in autons.cpp. That's it -- the selector picks it up automatically.

#pragma once

// Lets the driver pick an autonomous routine using the controller while the
// robot is disabled (sitting on the field waiting for the match to start).
// Call this once from pre_auton(). It returns as soon as the field enables
// the robot, so it can never delay a real match start.
void autonSelectorRun(void);

// Runs whichever autonomous routine was chosen by autonSelectorRun(). Call
// this from the autonomous() competition callback.
void runSelectedAuton(void);
