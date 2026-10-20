/*----------------------------------------------------------------------------*/
/*  robot-config.cpp - Hardware setup                                         */
/*                                                                            */
/*  This is the one place that says "what is plugged into which port".        */
/*  If you rewire your robot, this is the only file you should need to edit.  */
/*  (Matching `extern` declarations live in include/robot-config.h.)          */
/*                                                                            */
/*  PORT MAP                                                                  */
/*    Smart port 1  : cascade_lift_motor                                      */
/*    Smart port 12 : right_drive_motor  (reversed)                           */
/*    Smart port 20 : left_drive_motor                                        */
/*    3-wire port A : wrist_servo                                             */
/*    3-wire port B : claw_servo                                              */
/*----------------------------------------------------------------------------*/

#include "robot-config.h"

// The Brain itself. Needed to use the screen, battery info, etc.
vex::brain Brain;

// The driver's controller
vex::controller Controller1;

// ---- Motors (Motor Controller 55) ------------------------------------------
// The right motor is mounted facing the opposite direction of the left one,
// so it is "reversed" (second argument = true). This makes a positive voltage
// drive BOTH sides forward.
vex::motor55 left_drive_motor   = vex::motor55(vex::PORT20);
vex::motor55 right_drive_motor  = vex::motor55(vex::PORT12, true); // Reversed
vex::motor55 cascade_lift_motor = vex::motor55(vex::PORT1);

// ---- Servos ----------------------------------------------------------------
vex::servo wrist_servo = vex::servo(Brain.ThreeWirePort.A);
vex::servo claw_servo  = vex::servo(Brain.ThreeWirePort.B);
