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

extern vex::brain Brain;                           // The V5 Brain (screen, battery, ...)
extern vex::motor55 motor1;                        // 5.5W motor on the Brain
extern vex::aivision visionSensor;                 // AI Vision sensor
extern vex::aivision::colordesc AIVISION_RED_SIG;  // "What counts as red" definition
