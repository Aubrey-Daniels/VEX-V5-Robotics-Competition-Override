// robot-config.cpp
//
// Creates the real hardware objects declared in robot-config.h, using the
// ports/reversals/voltage settings from config.h. This is the only file
// that should construct motor/sensor objects -- everything else just uses
// the extern references.

#include "vex.h"
#include "robot-config.h"
#include "config.h"

using namespace vex;

brain      Brain;
controller Controller1 = controller(primary);

// Each motor55 is built from: port, max voltage (a hard safety cap -- the
// motor will never be sent more than this many volts even if software
// tries to), and "is this motor reversed on this side of the drivetrain"
// (see config.h for why that matters).
motor55 LeftDrive  = motor55(PORT_LEFT_DRIVE,  MAX_DRIVE_VOLTAGE, LEFT_MOTOR_REVERSED);
motor55 RightDrive = motor55(PORT_RIGHT_DRIVE, MAX_DRIVE_VOLTAGE, RIGHT_MOTOR_REVERSED);

// The servo constructor needs the 3-wire port object, not a letter. The
// letter in config.h is the thing you edit; this switch is the lookup.
// Brain is created just above, which the servo API requires -- the port
// belongs to the Brain.
static triport::port &servoThreeWirePort(void) {
  switch (SERVO_3WIRE_PORT) {
    case 'B':
    case 'b':
      return Brain.ThreeWirePort.B;
    case 'C':
    case 'c':
      return Brain.ThreeWirePort.C;
    case 'D':
    case 'd':
      return Brain.ThreeWirePort.D;
    case 'E':
    case 'e':
      return Brain.ThreeWirePort.E;
    case 'F':
    case 'f':
      return Brain.ThreeWirePort.F;
    case 'G':
    case 'g':
      return Brain.ThreeWirePort.G;
    case 'H':
    case 'h':
      return Brain.ThreeWirePort.H;
    case 'A':
    case 'a':
    default:
      return Brain.ThreeWirePort.A;
  }
}

servo TestServo = servo(servoThreeWirePort());

#if USE_INERTIAL_SENSOR
inertial Inertial = inertial(PORT_INERTIAL);
#endif

#if USE_DRIVE_ROTATION_SENSORS
rotation LeftRotation  = rotation(PORT_LEFT_ROTATION);
rotation RightRotation = rotation(PORT_RIGHT_ROTATION);
#endif

// `Competition` itself is defined in main.cpp (that's the file that wires
// it to the autonomous()/usercontrol() callbacks), but it's declared
// `extern` in robot-config.h so any file can check match state, e.g.
// `Competition.isEnabled()`.

void robotConfigInit(void) {
  // The Motor Controller 55 only supports "coast" (free-spin to a stop)
  // or "brake" (quick stop) -- unlike a Smart Motor, it has no "hold"
  // mode, since holding a position requires the kind of closed-loop
  // control a plain DC motor + MC55 pair can't do on its own. Coast
  // feels most natural for driver control.
  LeftDrive.setStopping(coast);
  RightDrive.setStopping(coast);
}
