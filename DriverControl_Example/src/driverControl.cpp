#include "vex.h"
#include "robot-config.h"

using namespace vex;

// Main driver control function using voltage (Volts)
int driveRobot_tankConfiguration() {

    while(1){
    // Read joystick values (-100 to 100 percent)
    double leftPercent = Controller1.Axis3.position(percent);
    double rightPercent = Controller1.Axis2.position(percent);

    // Convert percentage to volts (Max 12.0 V for V5 systems)
    double leftVolts = (leftPercent / 100.0) * 12.0;
    double rightVolts = (rightPercent / 100.0) * 12.0;

    // Control left drive motor using volts
    if (abs(leftVolts) > 0.1) { // Small threshold to prevent stick drift
        left_drive_motor.spin(fwd, leftVolts, volt);
    } else {
        left_drive_motor.stop();
    }

    // Control right drive motor using volts
    if (abs(rightVolts) > 0.1) {
        right_drive_motor.spin(fwd, rightVolts, volt);
    } else {
        right_drive_motor.stop();
    }


    wait(20, msec); // Small delay to prevent CPU overload
    }


    return 0; // This line will never be reached, but is included for completeness
}