/*----------------------------------------------------------------------------*/
/*  robot-config.h - Declarations of the robot's hardware                     */
/*                                                                            */
/*  The hardware objects are CREATED once in src/robot-config.cpp. The        */
/*  `extern` lines below let every other file USE those same objects by       */
/*  including this header. If you add a motor or sensor, add it in both       */
/*  places.                                                                   */
/*----------------------------------------------------------------------------*/
#pragma once

#include "vex.h"

// ---- Brain and Controller --------------------------------------------------
extern vex::brain Brain;               // The V5 Brain (screen, battery, ...)
extern vex::controller Controller1;    // The primary (driver) controller

// ---- Motors (plugged into the Brain's smart ports via Motor Controller 55) -
extern vex::motor55 left_drive_motor;      // Left side of the drivebase
extern vex::motor55 right_drive_motor;     // Right side of the drivebase (reversed)
extern vex::motor55 cascade_lift_motor;    // Raises / lowers the cascade lift

// ---- Servos (plugged into the Brain's 3-wire ports) ------------------------
extern vex::servo wrist_servo;             // Tilts the wrist (3-wire port A)
extern vex::servo claw_servo;              // Opens / closes the claw (3-wire port B)
