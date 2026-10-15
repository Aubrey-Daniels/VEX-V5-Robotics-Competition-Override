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

// A 5.5W (small) motor plugged into Brain port 5.
vex::motor55 motor1 = vex::motor55(vex::PORT5);

/*----------------------------------------------------------------------------*/
/*  AI Vision color signatures                                                */
/*                                                                            */
/*  A "color signature" tells the sensor what color to look for. You can      */
/*  create up to 7. The values come from the AI Vision Utility, where you can */
/*  point the sensor at an object and tune the numbers.                       */
/*----------------------------------------------------------------------------*/

// Red signature (this one is used in the example).
vex::aivision::colordesc AIVISION_RED_SIG(
    1,      // Signature ID (1-7). Each signature needs its own unique ID.
    255,    // Red value   (0-255)
    0,      // Green value (0-255)
    0,      // Blue value  (0-255)
    16.0,   // Hue range: how far from the target color still counts as a match
    0.30    // Saturation scale: how much saturation variation is tolerated
);

// Example of a second signature you could enable (currently unused):
/*
vex::aivision::colordesc AIVISION_TABLE_SIG(
    1, 137, 152, 105, 8.0, 0.30
);
*/

// The AI Vision sensor in Brain port 10. It is told to detect our red
// signature, AprilTags (ALL_TAGS), and the game's built-in AI objects
// (ALL_AIOBJS).
vex::aivision visionSensor(vex::PORT10,
                           AIVISION_RED_SIG,
                           vex::aivision::ALL_TAGS,
                           vex::aivision::ALL_AIOBJS);
