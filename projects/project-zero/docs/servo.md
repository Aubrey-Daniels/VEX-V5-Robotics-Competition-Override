# 3-wire servo practice

One VEX **3-wire servo** (`vex::servo`) sits next to the existing drivetrain. The drivetrain is still two DC motors on Motor Controller 55 (`vex::motor55`). This page does not replace that.

API checked against the VEX V5 C++ reference on Oct 14, 2026:

- [Servo Motor (C++)](https://api.vex.com/v5/home/cpp/Motors_and_MotorControllers/servo_motor.html)
- [Servo Motor (Blocks)](https://api.vex.com/v5/home/blocks/motion/servo_motor.html) — same device, and the page that states the degree travel

This project was **not compiled** here. The VEX VS Code extension's ARM toolchain is not installed in this environment. Build it once in VS Code (VEX: Build) before you trust it on a Brain.

## Servo vs the drive motors

| | 3-wire servo (`vex::servo`) | Drive motor (`vex::motor55`) |
|---|---|---|
| What you ask for | An angle. It goes there and **holds**. | A voltage. It **keeps spinning** until you stop it. |
| Connector | Lettered **3-Wire Port** (A–H) | Numbered **Smart Port** (the MC55) |
| How far it can move | A short arc, then it stops at the end of travel | Continuous rotation |
| Speed command | None. You only name the angle. | The voltage you pass to `spin` |
| Position you can read back | None in the current API. The screen shows the angle **we commanded**. | No encoder either. `current`, `voltage`, and `temperature` only. |

Use a servo for something that should sit at an angle (a gate, a hood, a little arm). Do not use it as a wheel.

## Wiring

1. Leave both Motor Controller 55 cables on their Smart Ports (`PORT_LEFT_DRIVE` / `PORT_RIGHT_DRIVE` in `include/config.h`).
2. Plug the servo's 3-wire connector into a **3-Wire Port** on the Brain. Ports are letters **A through H**, not the numbered Smart Ports.
3. The letter in code must match the socket. The default is port **A**:

```cpp
const char SERVO_3WIRE_PORT = 'A';  // include/config.h
```

Change that one letter if you move the cable. `src/robot-config.cpp` turns the letter into `Brain.ThreeWirePort.A` (or B–H) and builds the device:

```cpp
servo TestServo = servo(servoThreeWirePort());
```

The official constructor is `servo(triport::port &port)`. The Brain has to exist first; in this project `Brain` is created above `TestServo` in the same file. A 3-Wire Expander would be `ExpanderName.A` instead. This robot does not have one.

## Angle, center, and setPosition

The only member the C++ page lists is `setPosition`. Two overloads:

```cpp
void setPosition(int32_t value, percentUnits units);  // 0–100%
void setPosition(double value, rotationUnits units);  // degrees or turns
```

What the docs actually say about range:

- **Percent:** valid range is **0–100%** (C++ page).
- **Degrees:** the C++ page says this overload moves to the absolute position you give it. The Blocks page for this same motor says the horn travels **0 to 100 degrees** and holds there. Examples on the C++ page use `10` percent, `45.5` degrees, and `90` degrees.
- **Center:** both pages say a servo **centers itself at 50 degrees** when a project starts, before any other move. Keep fingers off the horn when you download.

This test uses **degrees**, clamped to 0–100, because center is specified in degrees. It does not assume that 50% and 50 degrees are the same command — the docs do not say that.

```cpp
TestServo.setPosition(50, degrees);   // center
TestServo.setPosition(0, degrees);     // one end
TestServo.setPosition(100, degrees);   // the other end
```

`setPosition` does not pause your program. The driver loop keeps running, so the robot can drive while the horn is moving.

Knobs you can change without touching the test logic, all in `include/config.h`:

| Constant | Default | Meaning |
|---|---|---|
| `SERVO_3WIRE_PORT` | `'A'` | Which 3-wire socket |
| `SERVO_MIN_DEG` / `SERVO_MAX_DEG` | 0 / 100 | Ends of travel |
| `SERVO_CENTER_DEG` | 50 | Center |
| `SERVO_JOG_DEG` | 5 | Degrees per Up or Down press |
| `SERVO_SWEEP_DEG_PER_SEC` | 10 | How fast ButtonB walks the horn |

## Button test

Code: `include/servo-tests.h`, `src/servo-tests.cpp`. `usercontrol()` in `src/main.cpp` calls `servoTestsInit()` once, then `servoTestsUpdate()` next to `driveControlUpdate()` every 20 ms.

**Up, Down, and A pick the auton while the robot is disabled.** They only move the servo after driver control starts. Don't judge the servo during the selector screen.

1. Build and download with the VEX extension, then run the program and enter **driver control**.
2. The Brain screen should show `3-wire servo on port A` and `Servo commanded: 50 deg (center)`. The horn should already be at center from program start; the test commands 50° again so the screen and the horn match.
3. **ButtonA** — go to center (50°). The screen prints `50 deg (center)`.
4. **ButtonUp** — add 5°. **ButtonDown** — subtract 5°. One press is one step (holding the button does not keep stepping). The command stays inside 0–100. The screen prints the new commanded angle.
5. **ButtonB** — start a slow sweep between 0° and 100°. The screen's number climbs and then falls (`sweep`). A full end-to-end pass is about 10 seconds at the default rate. **ButtonB again** stops and holds the current angle. **ButtonA**, **ButtonUp**, or **ButtonDown** also stop the sweep and do that button's move instead.
6. The joysticks still drive the whole time. Default is arcade (left stick forward/back, right stick turn). **ButtonX** still swaps tank and arcade. None of the servo buttons change the drive motors.

If the horn never moves: confirm the cable is in the letter `SERVO_3WIRE_PORT` names, that you are in driver control (not the disabled selector), and that the Brain screen's commanded angle changes when you press A. A changing number with a still horn is a cable or a dead servo. A number that never changes means driver control is not running this program.
