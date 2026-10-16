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

// Brain and Controller
extern vex::brain Brain;                           // The V5 Brain (screen, battery, ...)
extern vex::controller Controller1;                // The primary controller

// Motors
extern vex::motor55 right_drive_motor;              // Right drive motor
extern vex::motor55 left_drive_motor;               // Left drive motor