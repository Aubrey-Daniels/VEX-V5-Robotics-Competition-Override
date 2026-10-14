// config.h
//
// ============================================================================
//  ROBOT CONFIGURATION -- the ONE place to edit when something about the
//  physical robot changes.
// ============================================================================
//
// If your team moves a cable, flips a motor, or wants to tune autonomous
// driving, you should only ever need to change numbers in THIS file.
// Nothing in drive-control.*, auton-helpers.*, or autons.* should need to
// change for a normal port/wiring update.
//
// -----------------------------------------------------------------------
// Hardware this template assumes
// -----------------------------------------------------------------------
// Drivetrain: TWO legacy 2-wire DC motors, each driven through a VEX
// Motor Controller 55 (`vex::motor55`) -- NOT V5 Smart Motors (`vex::motor`).
// That matters a lot for what this file can and can't configure:
//   - No swappable gear cartridge -- MC55-driven DC motors don't have one,
//     so there is no "ratio6_1 / ratio18_1 / ratio36_1" setting here.
//   - No built-in encoder -- a Smart Motor silently tracks its own
//     rotation; an MC55 + DC motor combination reports NOTHING about how
//     far it has turned. There's physically no feedback signal unless
//     you bolt on a separate sensor yourself (see "OPTIONAL SENSORS"
//     near the bottom of this file).
// This is why driver control below is voltage/percent based, and why the
// example autonomous routines in autons.cpp are time-based ("drive at
// 70% power for 1.5 seconds") instead of distance-based.

#pragma once

#include "vex.h"

// ----------------------------------------------------------------------------
// 1. DRIVETRAIN PORTS
// ----------------------------------------------------------------------------
// Every VEX Motor Controller 55 plugs into a numbered V5 Smart Port
// (1-21) even though the DC motor on its other end is a simple 2-wire
// device -- the Brain talks to the MC55 itself over the Smart Port, and
// the MC55 relays power to the motor. If you move a cable, update the
// number here -- not anywhere else.

const int PORT_LEFT_DRIVE  = PORT1;
const int PORT_RIGHT_DRIVE = PORT2;

// ----------------------------------------------------------------------------
// 2. MOTOR REVERSALS
// ----------------------------------------------------------------------------
// Motors on opposite sides of a drivetrain are mirror images of each
// other. If you don't flip one side, commanding "both sides spin forward"
// will actually drive the robot in a circle. Set these to match how your
// motors are actually oriented -- if the robot drives backward when you
// push the stick forward, flip the matching value here.

const bool LEFT_MOTOR_REVERSED  = false;
const bool RIGHT_MOTOR_REVERSED = true;

// ----------------------------------------------------------------------------
// 3. DRIVE VOLTAGE
// ----------------------------------------------------------------------------
// A VEX Motor Controller 55 is driven with a voltage, not a Smart Motor
// style "velocity percent" -- it has no internal speed control loop, so
// "percent power" is really just "percent of the max voltage we're
// willing to send it." A standard VEX DC motor is rated for up to 12V.
//
// This value is used two ways: (1) as a hard safety cap passed into the
// motor55 constructor in robot-config.cpp, so nothing in software can
// ever command more voltage than the motor is rated for, and (2) as the
// "100%" reference every percent-based command (joystick input, timed
// auton moves) is converted against. Lower it if you want a gentler
// overall top speed without touching any other code.

const double MAX_DRIVE_VOLTAGE = 12.0; // volts

// ----------------------------------------------------------------------------
// 4. DRIVER CONTROL FEEL
// ----------------------------------------------------------------------------

// Drive mode: how joystick axes map to left/right motor power.
//   TANK   -> left stick drives the left side, right stick drives the right
//             side. Simple and precise, but turning uses two hands.
//   ARCADE -> one stick's forward/back axis drives speed, the other stick's
//             left/right axis steers. Easier for new drivers; closer to a
//             video-game feel.
enum class DriveMode { TANK, ARCADE };

// Beginners usually find ARCADE easier to learn; switch to TANK once your
// driver wants finer control. The controller button below lets a driver
// swap modes live without reflashing code (see drive-control.cpp).
const DriveMode DEFAULT_DRIVE_MODE = DriveMode::ARCADE;

// Joystick "deadband": every joystick drifts slightly off of zero even when
// you let go of it. Without a deadband, the robot could creep on its own.
// Any stick reading smaller than this (in percent, -100 to 100) is treated
// as exactly zero. Raise this if your controller's sticks are extra noisy;
// lower it if the robot feels unresponsive right off centre.
const int JOYSTICK_DEADBAND = 5; // percent

// ----------------------------------------------------------------------------
// 5. AUTONOMOUS -- TIME-BASED MOVES
// ----------------------------------------------------------------------------
// Without an encoder, the only way to drive "a certain distance" with the
// stock hardware is to drive at a known, repeatable power for a measured
// number of seconds -- the same way you'd time a toy car. It is NOT as
// accurate as feedback-based control (battery voltage sag, carpet vs.
// tile, and a dozen other things change how far the robot actually goes
// for the same time), but it is simple, requires no extra parts, and is a
// completely standard way to run VEX autonomous on this hardware.
//
// Lower this if your autonomous moves feel too aggressive/hard to time
// consistently; the example autons in autons.cpp use this as their
// default driving power.
const double AUTON_DRIVE_POWER_PERCENT = 70.0; // percent of MAX_DRIVE_VOLTAGE

// ----------------------------------------------------------------------------
// 6. OPTIONAL SENSORS -- only relevant if you add hardware beyond the two
//    drive motors described above.
// ----------------------------------------------------------------------------
// MC55-driven DC motors give you NO feedback about distance travelled or
// heading on their own. If your team wants real feedback-based ("closed
// loop") autonomous -- the kind that actively corrects itself instead of
// just guessing a time -- you need to bolt on separate sensors and wire
// them to open Smart Ports. The two switches below let you do that
// without touching any other file: flip a switch to 1, fill in the port
// number, and the matching helper functions in auton-helpers.* light up.
//
// Until you do that, leave both at 0. Nothing is assumed to be present.

// --- Heading feedback (for an accurate turnToHeading() move) ---------------
// Needs: one VEX Inertial Sensor (IMU), any open Smart Port.
#define USE_INERTIAL_SENSOR 0
const int PORT_INERTIAL = PORT10;

// PID gains for turnToHeading() -- only compiled/used when
// USE_INERTIAL_SENSOR is 1. See the README's tuning section for how to
// set these: tune kP first, then kD, and only add a small kI if the
// robot consistently stops a little short/past every time.
const double TURN_KP = 0.4;
const double TURN_KI = 0.0;
const double TURN_KD = 2.0;
const double TURN_TOLERANCE_DEG = 1.5; // "close enough" to stop turning
const double TURN_TIMEOUT_SEC   = 3.0; // safety cutoff
const double TURN_MAX_PERCENT   = 70.0; // caps PID output

// --- Distance feedback (for an accurate driveStraight() move) --------------
// Needs: two VEX Rotation Sensors, one per side, mounted on (or geared to)
// a wheel axle -- any open Smart Ports. A plain DC motor has no shaft
// encoder of its own, so this is the only way to measure how far an
// MC55-driven wheel has actually turned.
#define USE_DRIVE_ROTATION_SENSORS 0
const int PORT_LEFT_ROTATION  = PORT11;
const int PORT_RIGHT_ROTATION = PORT12;

// How the Rotation Sensor's shaft relates to the wheel it's measuring --
// same ideas as a drivetrain gear ratio, just applied to the sensor
// instead of a motor cartridge. If the sensor is mounted directly on the
// wheel axle (most common), leave this at 1.0.
const double ROTATION_SENSOR_GEAR_RATIO = 1.0; // sensor turns per wheel turn
const double WHEEL_DIAMETER_IN = 3.25;         // inches

// PID gains for driveStraight() -- only compiled/used when
// USE_DRIVE_ROTATION_SENSORS is 1.
const double DRIVE_KP = 0.4;
const double DRIVE_KI = 0.0;
const double DRIVE_KD = 2.5;
const double DRIVE_TOLERANCE_IN = 0.5; // "close enough" to stop driving
const double DRIVE_TIMEOUT_SEC  = 4.0; // safety cutoff
const double DRIVE_MAX_PERCENT  = 80.0; // caps PID output

// ----------------------------------------------------------------------------
// 7. 3-WIRE SERVO -- a practice device, separate from the drivetrain
// ----------------------------------------------------------------------------
// A VEX 3-wire servo holds an angle. It is not a drive motor and it does
// not replace the Motor Controller 55 units above. Those stay on numbered
// Smart Ports. A servo plugs into a lettered 3-Wire Port (A-H) on the
// Brain. Change the letter here if you move that cable; robot-config.cpp
// is the only place that letter is turned into a port.
//
// Travel, from the current VEX docs (see docs/servo.md): 0 degrees at one
// end of the horn's travel, 100 degrees at the other, 50 degrees at the
// center. The servo also jumps to that center by itself when a program
// starts. There is no speed argument -- the tests below walk the angle
// in small steps when a slow sweep is what you want.

const char SERVO_3WIRE_PORT = 'A';

const double SERVO_MIN_DEG = 0.0;
const double SERVO_MAX_DEG = 100.0;
const double SERVO_CENTER_DEG = 50.0;

// One press of ButtonUp / ButtonDown moves the horn by this many degrees.
const double SERVO_JOG_DEG = 5.0;

// ButtonB walks the horn from end to end at this rate. The driver-control
// loop waits 20 ms, and the sweep uses that wait as its clock.
const double SERVO_SWEEP_DEG_PER_SEC = 10.0;
