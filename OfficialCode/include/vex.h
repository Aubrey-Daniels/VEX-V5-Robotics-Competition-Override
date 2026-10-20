/*----------------------------------------------------------------------------*/
/*  vex.h - Common includes and helper macros                                 */
/*                                                                            */
/*  Every other file in the project includes this header first. It pulls in   */
/*  the C standard library and the VEX V5 library, and defines two small      */
/*  helper macros.                                                            */
/*----------------------------------------------------------------------------*/
#pragma once   // Prevents this file from being included twice in one build

// C standard library headers (math, input/output, memory, strings)
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// VEX V5 library headers (motors, servos, controller, brain, competition...)
#include "v5.h"
#include "v5_vcs.h"

// waitUntil(condition)
// Pauses the program until `condition` becomes true, checking every 5 ms.
// Example: waitUntil(Controller1.ButtonA.pressing());
#define waitUntil(condition)                                                   \
  do {                                                                         \
    wait(5, msec);                                                             \
  } while (!(condition))

// repeat(n)
// Runs the following block of code n times.
// Example: repeat(3) { Brain.Screen.print("hi"); }
#define repeat(iterations)                                                     \
  for (int iterator = 0; iterator < iterations; iterator++)
