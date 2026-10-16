/*----------------------------------------------------------------------------*/
/*  vex.h - Master include file                                               */
/*                                                                            */
/*  Every .cpp file in this project includes this one first. It pulls in the  */
/*  standard C libraries plus the VEX V5 libraries (v5.h / v5_vcs.h), which   */
/*  give you motors, sensors, the Brain screen, timers and so on.             */
/*  This file comes from the VEX extension's project template.                */
/*----------------------------------------------------------------------------*/
#pragma once

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "v5.h"
#include "v5_vcs.h"

// Convenience macro: pause the program until `condition` becomes true.
// Example: waitUntil(Controller1.ButtonA.pressing());
#define waitUntil(condition)                                                   \
  do {                                                                         \
    wait(5, msec);                                                             \
  } while (!(condition))

// Convenience macro: run a block of code a set number of times.
// Example: repeat(3) { Brain.Screen.print("Hi"); }
#define repeat(iterations)                                                     \
  for (int iterator = 0; iterator < iterations; iterator++)
