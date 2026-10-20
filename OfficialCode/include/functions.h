/*----------------------------------------------------------------------------*/
/*  functions.h - Subsystem control functions                                 */
/*                                                                            */
/*  Each function below is an infinite loop that controls ONE subsystem.      */
/*  main.cpp starts each one as its own parallel task so they all run at      */
/*  the same time without blocking each other.                                */
/*----------------------------------------------------------------------------*/
#pragma once

#include "vex.h"

int driveRobot_tankConfiguration();  // Drivebase  - driverControl.cpp
int cascadeLiftControl();            // Lift       - cascadeLiftControl.cpp
int wristControl();                  // Wrist      - wristControl.cpp
int clawControl();                   // Claw       - clawControl.cpp
