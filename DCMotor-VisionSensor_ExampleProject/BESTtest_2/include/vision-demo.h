/*----------------------------------------------------------------------------*/
/*  vision-demo.h - Declarations for the AI Vision example                    */
/*                  (src/vision-demo.cpp)                                     */
/*----------------------------------------------------------------------------*/
#pragma once

#include "vex.h"

// Background task: continuously looks for red objects and reports them on the
// Brain screen. Start it with:  vex::task myTask(visionSensorFunction);
int visionSensorFunction();
