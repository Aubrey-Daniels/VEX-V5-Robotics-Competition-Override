# Robot Code Documentation (VEX V5, C++)

**Author:** zachb_d  **Created:** 10/15/2026  **Documented:** 10/20/2026

## 1. Overview

This program runs a VEX V5 robot with four subsystems, all operated by one driver on a single controller:

| Subsystem | What it does | Hardware | File |
|---|---|---|---|
| Drivebase | Tank drive (one stick per side) | 2 motors via Motor Controller 55 | `driverControl.cpp` |
| Cascade lift | Raises and lowers the lift | 1 motor via Motor Controller 55 | `cascadeLiftControl.cpp` |
| Wrist | Tilts the claw assembly | 1 servo | `wristControl.cpp` |
| Claw | Opens / closes | 1 servo | `clawControl.cpp` |

Autonomous mode is not implemented yet (`autonomous()` in `main.cpp` is an empty placeholder).

## 2. Driver Controls

| Input | Action |
|---|---|
| Left joystick (up/down, Axis 3) | Left side of drivebase |
| Right joystick (up/down, Axis 2) | Right side of drivebase |
| Hold **R1** | Lift up |
| Hold **R2** | Lift down |
| Press **L1** | Toggle wrist between 0° and 90° |
| Press **L2** | Toggle claw between 0° and 90° |

*Hold* = moves only while the button is held. *Press* = one tap flips the position.

## 3. Hardware Map

| Device | Name in code | Port | Notes |
|---|---|---|---|
| Left drive motor | `left_drive_motor` | Smart port 20 | |
| Right drive motor | `right_drive_motor` | Smart port 12 | **Reversed** so positive voltage = forward |
| Cascade lift motor | `cascade_lift_motor` | Smart port 1 | |
| Wrist servo | `wrist_servo` | 3-wire port A | |
| Claw servo | `claw_servo` | 3-wire port B | |

All devices are created in `robot-config.cpp` and shared with the rest of the program through `robot-config.h`.

## 4. Program Architecture

```
main()
 ├─ registers autonomous() and usercontrol() with the Competition object
 ├─ calls pre_auton()   (one-time setup, currently empty)
 └─ idles forever

usercontrol()   (runs during driver control)
 ├─ task: driveRobot_tankConfiguration()
 ├─ task: cascadeLiftControl()
 ├─ task: wristControl()
 └─ task: clawControl()
```

**Why tasks?** Each subsystem is an infinite loop in its own parallel task. Because they run independently, a slow or waiting subsystem never delays another, so the robot can drive, lift and grab at the same moment. Every loop ends with `wait(20, msec)`, which hands processor time to the other tasks and refreshes inputs 50 times per second.

### File structure

```
include/
  vex.h             common includes + helper macros (waitUntil, repeat)
  robot-config.h    extern declarations of all hardware
  functions.h       prototypes of the four subsystem functions
src/
  main.cpp          entry point, competition setup, task startup
  robot-config.cpp  creates the hardware objects (port map)
  driverControl.cpp
  cascadeLiftControl.cpp
  wristControl.cpp
  clawControl.cpp
```

## 5. Subsystem Details

### 5.1 Drivebase (tank drive)
Each loop:
1. Read both joysticks (−100 to +100 %).
2. Convert to voltage: `volts = percent / 100 × 12`.
3. If |volts| > 0.1 V, spin the motor at that voltage; otherwise stop it. This **deadband** prevents motor creep when a stick does not rest exactly at center.

Voltage control (rather than percent velocity) keeps motor output consistent as the battery drains.

### 5.2 Cascade lift
Simple "hold to move" logic: R1 → forward at 12 V, R2 → reverse at 12 V, neither → stop. R1 is checked first, so it wins if both are held. The lift has no limit switch or position sensor, so the driver must avoid driving it past its mechanical limits.

### 5.3 Wrist and claw (servo toggles)
Both use the same **edge detection** technique:

```
buttonPressed  = is the button down right now?
previousButton = was it down on the last loop?

if (buttonPressed AND NOT previousButton):   # new press only
    flip state, move servo to the new angle
previousButton = buttonPressed
```

Without this check, holding the button would flip the servo every 20 ms. The servos start the program at an unknown position, then jump to 90° on the first press and back to 0° on the second.

## 6. Tunable Settings

Each file begins with `static const` values so you never need to hunt through the logic:

| Setting | File | Default | Meaning |
|---|---|---|---|
| `MAX_VOLTAGE` | driverControl.cpp | 12.0 | Full drive power (V) |
| `DEADBAND_VOLTS` | driverControl.cpp | 0.1 | Joystick dead zone (V) |
| `LIFT_VOLTAGE` | cascadeLiftControl.cpp | 12.0 | Lift power (V) |
| `WRIST_DOWN_DEGREES` / `WRIST_UP_DEGREES` | wristControl.cpp | 0 / 90 | Wrist positions |
| `CLAW_POSITION_A_DEGREES` / `_B_` | clawControl.cpp | 0 / 90 | Claw positions |
| `LOOP_DELAY_MS` | each subsystem | 20 | Loop period (ms) |

## 7. Reusing This Code

**Change a port:** edit only `robot-config.cpp`.

**Add a new motor or sensor:**
1. Create it in `robot-config.cpp`.
2. Add the matching `extern` line in `robot-config.h`.

**Add a new subsystem:**
1. Create `mySubsystem.cpp` with an `int mySubsystem()` function containing a `while(1)` loop ending in `wait(20, msec)`.
2. Declare it in `functions.h`.
3. Start it in `usercontrol()` with `vex::task myTask(mySubsystem);`.

**Change a button:** edit the `Controller1.ButtonXX` line in the subsystem's file (for example `ButtonL1` → `ButtonA`).

**Flip a direction:** change the `true`/`false` reversed flag in `robot-config.cpp`, or swap `forward`/`reverse` in the subsystem file.

**Servo angles:** set the two position constants to whatever matches your mechanism. Test with small steps first to avoid binding the servo against a hard stop.

## 8. Changes Made During Documentation

Behavior is unchanged except for item 1.

1. **Bug fix (driverControl.cpp):** `abs()` was used on decimal values. In some C++ environments `abs()` converts to an integer first, which would turn the 0.1 V deadband into roughly 1 V. It now uses `fabs()`, and the duplicated left/right logic is in one helper, `driveSide()`.
2. Added `#pragma once` to `vex.h` so it cannot be included twice.
3. Replaced magic numbers (12.0, 90, 0, 20) with named constants.
4. Added file headers, port map, controls and comments to every file.
5. Renamed `clawUp` to `clawToggled` in `clawControl.cpp` (the claw is open/closed, not up/down) and added its missing `functions.h` include.
6. Cleaned out unused template comments in `main.cpp`.

## 9. Known Limitations / Ideas for Improvement

- **No autonomous routine.** A good first step is timed driving, then PID control with encoders (the V5 motors have built-in encoders for distance/heading feedback).
- **Lift has no feedback.** Adding a limit switch or using the motor's encoder would give soft position limits and "hold" presets. Setting `cascade_lift_motor.setStopping(hold)` could also keep it from sliding down under load.
- **Servo start position is not set.** Calling `setPosition(0, degrees)` for both servos in `pre_auton()` would make the toggle state match the real position at startup.
- **Drive has no input shaping.** A squared or cubic joystick curve gives finer low-speed control.
- **Tasks are not stopped** when driver control ends; this is fine for a single match but worth handling for longer programs.
