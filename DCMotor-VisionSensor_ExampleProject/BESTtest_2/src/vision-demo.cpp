/*----------------------------------------------------------------------------*/
/*  vision-demo.cpp - Example: reading the AI Vision sensor                   */
/*                                                                            */
/*  Runs as its own task (a function running "in parallel" with main), so the */
/*  sensor can be polled without slowing down the rest of the program.        */
/*----------------------------------------------------------------------------*/

#include "vex.h"
#include "vision-demo.h"
#include "robot-config.h"  // gives us Brain, visionSensor, AIVISION_RED_SIG

using namespace vex;

// Task function: started from main() with `vex::task`.
int visionSensorFunction() {
  // Give the sensor a few seconds to finish starting up.
  wait(3, sec);

  // installed() is true if the sensor is plugged in and responding.
  if (visionSensor.installed()) {
    Brain.Screen.setCursor(1, 1);  // row 1, column 1
    Brain.Screen.print("AI Vision Connected!");
  }

  // Loop forever, checking for red objects.
  while (true) {
    // Ask the sensor to capture one frame and look for our red signature.
    // The results are stored in visionSensor.objects[] and
    // visionSensor.objectCount.
    visionSensor.takeSnapshot(AIVISION_RED_SIG);

    // Move to row 2 and erase the old message before printing a new one.
    Brain.Screen.setCursor(2, 1);
    Brain.Screen.clearLine(2);

    if (visionSensor.objectCount > 0) {
      // objects[0] is the first (largest) match. .width is in pixels.
      Brain.Screen.print("Red Object Found! Width: %d",
                         visionSensor.objects[0].width);
    } else {
      Brain.Screen.print("No red object.");
    }

    // Wait 250 ms between checks so we don't hog the processor.
    wait(250, msec);
  }

  return 0;  // Never reached (the loop above runs forever), but tasks must
             // return an int.
}
