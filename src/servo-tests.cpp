// servo-tests.cpp
//
// Button test for one 3-wire servo. The horn only travels about
// 0 to 100 degrees and then holds wherever you leave it. 50 degrees is
// the center -- the same place the servo jumps to by itself when a
// program starts.
//
// There is no "spin at this speed" command on a servo. setPosition says
// where the horn should go, and the servo moves there on its own and
// holds. To sweep slowly, this file changes that target a little bit
// every time the driver-control loop comes around.
//
// The number on the Brain screen is the angle we commanded. The VEX
// servo API does not give a call that reads the horn back, so the screen
// cannot show a measured angle -- only the last request.

#include "vex.h"
#include "robot-config.h"
#include "servo-tests.h"
#include "config.h"

using namespace vex;

// Last angle we asked for, in degrees. Starts at center so the screen
// matches the position the servo takes at program start, even before
// the driver presses anything.
static double commandedDeg = SERVO_CENTER_DEG;
static bool sweeping = false;
static int sweepDirection = 1;

static void showCommanded(const char *state) {
  int shown = (int)(commandedDeg + 0.5);

  // The sweep updates the target many times a second. Reprint only when
  // the degree the driver can read, or the word next to it, changes.
  static int lastShown = -1;
  static const char *lastState = "";
  if (shown == lastShown && lastState == state) {
    return;
  }
  lastShown = shown;
  lastState = state;

  Brain.Screen.setCursor(3, 1);
  Brain.Screen.clearLine();
  Brain.Screen.print("Servo commanded: %d deg (%s)", shown, state);

  Brain.Screen.setCursor(4, 1);
  Brain.Screen.clearLine();
  Brain.Screen.print("A center   Up/Down %d deg   B sweep",
                     (int)SERVO_JOG_DEG);
}

// Ask the servo to hold `deg`, keeping the request inside the documented
// travel. Past either end the horn is already against its stop; asking
// for more does not buy extra range.
static void commandServo(double deg, const char *state) {
  if (deg < SERVO_MIN_DEG) {
    deg = SERVO_MIN_DEG;
  }
  if (deg > SERVO_MAX_DEG) {
    deg = SERVO_MAX_DEG;
  }

  commandedDeg = deg;
  TestServo.setPosition(commandedDeg, degrees);
  showCommanded(state);
}

static void onServoCenter(void) {
  sweeping = false;
  commandServo(SERVO_CENTER_DEG, "center");
}

static void onServoJogUp(void) {
  sweeping = false;
  commandServo(commandedDeg + SERVO_JOG_DEG, "jog up");
}

static void onServoJogDown(void) {
  sweeping = false;
  commandServo(commandedDeg - SERVO_JOG_DEG, "jog down");
}

// ButtonB starts a slow end-to-end sweep, or stops one that is already
// running and leaves the horn where it is. A manual button (A, Up, Down)
// also stops the sweep, because those are "go to this angle" requests.
static void onServoSweep(void) {
  if (sweeping) {
    sweeping = false;
    showCommanded("hold");
    return;
  }

  sweeping = true;
  if (commandedDeg >= SERVO_MAX_DEG) {
    sweepDirection = -1;
  } else {
    sweepDirection = 1;
  }
  showCommanded("sweep");
}

void servoTestsInit(void) {
  static bool ready = false;
  if (ready) {
    return;
  }
  ready = true;

  sweeping = false;
  commandedDeg = SERVO_CENTER_DEG;
  TestServo.setPosition(commandedDeg, degrees);

  Brain.Screen.clearScreen();
  Brain.Screen.setCursor(1, 1);
  Brain.Screen.print("Joysticks drive. ButtonX swaps mode.");
  Brain.Screen.setCursor(2, 1);
  Brain.Screen.print("3-wire servo on port %c", SERVO_3WIRE_PORT);
  showCommanded("center");

  // Each of these fires once per press, not the whole time the button
  // is held. One press is one small horn move, so you can watch it.
  Controller1.ButtonA.pressed(onServoCenter);
  Controller1.ButtonUp.pressed(onServoJogUp);
  Controller1.ButtonDown.pressed(onServoJogDown);
  Controller1.ButtonB.pressed(onServoSweep);
}

void servoTestsUpdate(void) {
  if (!sweeping) {
    return;
  }

  // The driver loop waits 20 ms (see usercontrol in main.cpp). That
  // wait is the clock for the sweep: a few degrees per second, not a
  // snap from one end to the other. setPosition does not pause, so the
  // drivetrain still gets new joystick commands on this same pass.
  double step = SERVO_SWEEP_DEG_PER_SEC * 0.020;
  double next = commandedDeg + (sweepDirection * step);

  if (next >= SERVO_MAX_DEG) {
    next = SERVO_MAX_DEG;
    sweepDirection = -1;
  } else if (next <= SERVO_MIN_DEG) {
    next = SERVO_MIN_DEG;
    sweepDirection = 1;
  }

  commandServo(next, "sweep");
}
