/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       zachb_d                                                   */
/*    Created:      10/15/2026, 7:08:55 PM                                    */
/*    Description:  V5 project - program entry point                          */
/*                                                                            */
/*  HOW THE PROGRAM IS ORGANIZED                                              */
/*    main()        -> registers the competition callbacks, then idles.       */
/*    pre_auton()   -> runs once at power-on (setup).                         */
/*    autonomous()  -> runs during the autonomous period (currently empty).   */
/*    usercontrol() -> runs during driver control. It starts one parallel     */
/*                     task per subsystem (drive, lift, wrist, claw).         */
/*----------------------------------------------------------------------------*/

#include "vex.h"
#include "robot-config.h"
#include "functions.h"

using namespace vex;

// Object that talks to the field controller / competition switch.
competition Competition;

/*---------------------------------------------------------------------------*/
/*  Pre-Autonomous                                                           */
/*  Runs once after the V5 is powered on (NOT every time the robot is        */
/*  disabled). Put setup here, e.g. resetting sensors or servo positions.    */
/*  It must return, or autonomous and driver control will never start.       */
/*---------------------------------------------------------------------------*/
void pre_auton(void) {
  // Nothing to set up yet.
}

/*---------------------------------------------------------------------------*/
/*  Autonomous                                                               */
/*  Runs for the autonomous period of a match. Not yet implemented.          */
/*---------------------------------------------------------------------------*/
void autonomous(void) {
  // TODO: add autonomous routine here.
}

/*---------------------------------------------------------------------------*/
/*  Driver Control                                                           */
/*  Each subsystem runs in its own task, so one subsystem can never block    */
/*  another (for example, the lift can move while the robot drives).         */
/*---------------------------------------------------------------------------*/
void usercontrol(void) {

  // Start one parallel task per subsystem. Each task loops forever.
  vex::task driveRobot_tankConfigurationTask(driveRobot_tankConfiguration);
  vex::task cascadeLiftControlTask(cascadeLiftControl);
  vex::task wristControlTask(wristControl);
  vex::task clawControlTask(clawControl);

  // Keep this function alive. If it returned, driver control would end.
  while (1) {
    wait(20, msec); // Yield so other tasks get processor time
  }
}

/*---------------------------------------------------------------------------*/
/*  Main                                                                     */
/*  Registers the competition callbacks and runs setup.                      */
/*---------------------------------------------------------------------------*/
int main() {
  // Tell the competition system which function to run in each period.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  // Run one-time setup.
  pre_auton();

  // Prevent main from exiting.
  while (true) {
    wait(100, msec);
  }
}
