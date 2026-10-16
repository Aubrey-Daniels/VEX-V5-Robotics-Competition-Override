/*----------------------------------------------------------------------------*/
/*  robot-config.cpp - Hardware setup                                         */
/*                                                                            */
/*  This is the one place that says "what is plugged into which port".        */
/*  If you rewire your robot, this is the only file you should need to edit.  */
/*  (Matching `extern` declarations live in include/robot-config.h.)          */
/*----------------------------------------------------------------------------*/

#include "robot-config.h"

// The Brain itself. Needed to use the screen, battery info, etc.
vex::brain Brain;
vex::controller Controller1; // The primary controller

// Motor 55 Controllers
vex::motor55 left_drive_motor = vex::motor55(vex::PORT20);
vex::motor55 right_drive_motor = vex::motor55(vex::PORT12, true);