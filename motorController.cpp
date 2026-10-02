#include "main.h"
#include "motorController.h"

// Motor declarations
pros::Motor leftMotor(LEFT_MOTOR_PORT);
pros::Motor rightMotor(RIGHT_MOTOR_PORT);
pros::Motor intakeMotor(INTAKE_MOTOR_PORT);
pros::Motor launcherMotor(LAUNCHER_MOTOR_PORT);

/**
 * Test a single motor at a specified power for a duration
 * @param motor The motor to test
 * @param power The power level (-127 to 127)
 * @param duration The duration in milliseconds
 */
void testMotor(pros::Motor& motor, int power, int duration) {
    std::cout << "Testing motor at power: " << power << std::endl;
    motor.move(power);
    pros::delay(duration);
    motor.move(0);
    std::cout << "Motor test complete" << std::endl;
}

/**
 * Test all motors sequentially
 */
void testAllMotors() {
    std::cout << "Starting motor tests..." << std::endl;
    
    // Test left motor
    std::cout << "\n--- Testing Left Motor ---" << std::endl;
    testMotor(leftMotor, 100, 1000);
    pros::delay(500);
    
    // Test right motor
    std::cout << "\n--- Testing Right Motor ---" << std::endl;
    testMotor(rightMotor, 100, 1000);
    pros::delay(500);
    
    // Test intake motor
    std::cout << "\n--- Testing Intake Motor ---" << std::endl;
    testMotor(intakeMotor, 100, 1000);
    pros::delay(500);
    
    // Test launcher motor
    std::cout << "\n--- Testing Launcher Motor ---" << std::endl;
    testMotor(launcherMotor, 100, 1000);
    pros::delay(500);
    
    std::cout << "\nAll motor tests complete!" << std::endl;
}

/**
 * Drive forward at specified power for a duration
 * @param power The power level (0 to 127)
 * @param duration The duration in milliseconds
 */
void driveForward(int power, int duration) {
    std::cout << "Driving forward at power: " << power << std::endl;
    leftMotor.move(power);
    rightMotor.move(power);
    pros::delay(duration);
    stopAllMotors();
}

/**
 * Drive backward at specified power for a duration
 * @param power The power level (0 to 127)
 * @param duration The duration in milliseconds
 */
void driveBackward(int power, int duration) {
    std::cout << "Driving backward at power: " << power << std::endl;
    leftMotor.move(-power);
    rightMotor.move(-power);
    pros::delay(duration);
    stopAllMotors();
}

/**
 * Turn left at specified power for a duration
 * @param power The power level (0 to 127)
 * @param duration The duration in milliseconds
 */
void turnLeft(int power, int duration) {
    std::cout << "Turning left at power: " << power << std::endl;
    leftMotor.move(-power);
    rightMotor.move(power);
    pros::delay(duration);
    stopAllMotors();
}

/**
 * Turn right at specified power for a duration
 * @param power The power level (0 to 127)
 * @param duration The duration in milliseconds
 */
void turnRight(int power, int duration) {
    std::cout << "Turning right at power: " << power << std::endl;
    leftMotor.move(power);
    rightMotor.move(-power);
    pros::delay(duration);
    stopAllMotors();
}

/**
 * Stop all motors
 */
void stopAllMotors() {
    std::cout << "Stopping all motors" << std::endl;
    leftMotor.move(0);
    rightMotor.move(0);
    intakeMotor.move(0);
    launcherMotor.move(0);
}
