#include main.h
#include motorController.h

// Motor objects
pros::Motor leftMotor(LEFT_MOTOR_PORT);
pros::Motor rightMotor(RIGHT_MOTOR_PORT);
pros::Motor intakeMotor(INTAKE_MOTOR_PORT);
pros::Motor launcherMotor(LAUNCHER_MOTOR_PORT);

// Stop all motors
void stopAllMotors() {
    leftMotor.move(0);
    rightMotor.move(0);
    intakeMotor.move(0);
    launcherMotor.move(0);
}

// Move a single motor for a set time (in milliseconds)
void moveMotor(pros::Motor& motor, int power, int durationMs) {
    motor.move(power);
    pros::delay(durationMs);
    motor.move(0);
}

// Drive the robot
void drive(int leftPower, int rightPower, int durationMs) {
    leftMotor.move(leftPower);
    rightMotor.move(rightPower);
    pros::delay(durationMs);
    stopAllMotors();
}

// Turn the robot
void turn(int power, int durationMs) {
    leftMotor.move(power);
    rightMotor.move(-power);
    pros::delay(durationMs);
    stopAllMotors();
}

// Test all motors one after another
void testAllMotors() {
    moveMotor(leftMotor, 100, 1000);
    pros::delay(500);
    moveMotor(rightMotor, 100, 1000);
    pros::delay(500);
    moveMotor(intakeMotor, 100, 1000);
    pros::delay(500);
    moveMotor(launcherMotor, 100, 1000);
    pros::delay(500);
}
