# VEX C++ SDK API Cheatsheet — What You'll Actually Use

Verified against the official [VEX V5 C++ API reference](https://api.vex.com/v5/home/cpp/index.html) (api.vex.com) — **not PROS**. Every class, constructor, and member function below is in the **`vex::` namespace**. `using namespace vex;` (generated in every new project) lets you drop the `vex::` prefix.

← [Learning path](vex-learning-path.md) · [Setup](vex-setup.md) · [Autonomous concepts](vex-autonomous-concepts.md) · [3-wire servo](servo.md)

**Include in every file:** `#include "vex.h"` (pulls in all VEX SDK headers + `using namespace vex;`).

> **If you're coming from a PROS tutorial:** names are different. `pros::Motor` → `vex::motor`/`vex::motor55` (see below — this robot uses the latter); `move_velocity()` → `spin()`/`setVelocity()`; `pros::Imu` → `vex::inertial`; `initialize/opcontrol` → `pre_auton/usercontrol`. Don't mix PROS code examples into a VEX SDK project — the two APIs are unrelated and PROS code will not compile here. (This repo contains no PROS code or PROS CLI config — if you see `pros::` anywhere, it's from an old example and should be rewritten against this cheatsheet, not run as-is.)
>
> **Hardware note:** this robot's drivetrain is **DC motors via Motor Controller 55**, not V5 Smart Motors — jump to [§ Primary Drive API: Motor Controller 55](#primary-drive-api-motor-controller-55-vexmotor55) first. The `vex::motor` section further down is an optional future-upgrade reference, not what this robot runs. See [project-context.md § Hardware](project-context.md#hardware-corrected-oct-8-2026-evening--authoritative).

---

## Competition template (`vex::competition`)

The template function names are **`pre_auton`**, **`autonomous`**, **`usercontrol`** (C++; Python uses different names — don't mix language docs).

```cpp
#include "vex.h"
using namespace vex;

competition Competition;

void pre_auton(void) {
  // Calibration, sensor resets — runs once at program start, before auton
}

void autonomous(void) {
  // Autonomous routine — no controller input allowed here
}

void usercontrol(void) {
  while (true) {
    // Driver control loop
    wait(20, msec);
  }
}

int main() {
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);
  pre_auton();
  while (true) {
    wait(100, msec);
  }
}
```

`competition` member functions:

| Function | Purpose |
|---|---|
| `autonomous(void (*callback)(void))` | Registers the function to run when the Autonomous period starts |
| `drivercontrol(void (*callback)(void))` | Registers the function to run when the Driver Control period starts |
| `isEnabled()` | `bool` — robot enabled under competition control |
| `isAutonomous()` / `isDriverControl()` | `bool` — current match phase |
| `isCompetitionSwitch()` | `bool` — competition switch connected |
| `isFieldControl()` | `bool` — field control system connected |

[Competition API](https://api.vex.com/v5/home/cpp/Competition.html) · [Competition Template article](https://kb.vex.com/hc/en-us/articles/360036286691-Using-the-Competition-Template-in-VEXcode-Pro-V5)

---

## Primary Drive API: Motor Controller 55 (`vex::motor55`)

**This robot's drivetrain uses DC motors through 2× VEX Motor Controller 55 (MC55) units — not V5 Smart Motors.** This is the API that matters for this robot today. The `vex::motor` (V5 Smart Motor) section further down is kept for reference as an **optional future-upgrade path only** — don't center drivetrain code on it.

[Motor Controller 55 API](https://api.vex.com/v5/home/cpp/Motors_and_MotorControllers/mc55.html)

### What's different from a V5 Smart Motor

| | `vex::motor` (Smart Motor) | `vex::motor55` (this robot) |
|---|---|---|
| Encoder / position feedback | Built in | **None** |
| `spinFor` / `spinToPosition` / `isDone` | Yes | **Not available** |
| Gear cartridges | Yes (`ratio36_1`/`18_1`/`6_1`) | N/A — plain 2-wire DC motor |
| `spin(dir, voltage, units)` range | Up to battery voltage (~12V) | **−8V to 8V** |
| Telemetry | position, velocity, current, power, torque, efficiency, temperature, voltage | **Only** `current()`, `voltage()`, `temperature()`, `getMotorType()` |
| Works with `drivetrain`/`smartdrive` built-in (`driveFor`, `turnFor`, `turnToHeading`) | Yes | **No** — those classes require `motor`/`motor_group` objects, not `motor55` |

### Constructors

```cpp
motor55(int32_t index);
motor55(int32_t index, bool reverse);
motor55(int32_t index, double maxv, bool reverse);  // caps max drive voltage
```

| Parameter | Detail |
|---|---|
| `index` | Smart Port, written `PORTx` (e.g. `PORT1`) — **MC55 plugs into a Smart Port**, like a Smart Motor, even though it drives a non-smart 2-wire DC motor |
| `reverse` | `false` (default) = normal; `true` = reversed |
| `maxv` | Optional cap on drive voltage, in volts |

```cpp
motor55 LeftDrive = motor55(PORT1);
motor55 RightDrive = motor55(PORT2, true);  // reversed
```

### Actions

| Method | Signature | Notes |
|---|---|---|
| `spin` | `void spin(directionType dir)` — uses configured velocity <br> `void spin(directionType dir, double voltage, voltageUnits units)` | Spins forever, non-waiting. Voltage range **−8V to 8V** (not full battery voltage). |
| `stop` | `void stop()` <br> `void stop(brakeType mode)` | **Only `coast` and `brake` are supported — `hold` is not.** |

### Mutators

| Method | Signature | Notes |
|---|---|---|
| `setVelocity` | `void setVelocity(double velocity, percentUnits units = percent)` | 0–100%. **Every project starts at 50% by default** — this alone makes motors feel sluggish until you raise it. |
| `setStopping` | `void setStopping(brakeType mode)` | Default for subsequent `stop()` calls; `coast` or `brake` only |
| `setReversed` | `void setReversed(bool reverse)` | Flip direction without re-constructing |

### Getters

| Method | Returns |
|---|---|
| `current(currentUnits units = amp)` | Electrical current draw |
| `voltage(voltageUnits units = volt)` | Supplied voltage |
| `temperature(percentUnits/temperatureUnits units = percent)` | Controller temperature |
| `getMotorType()` | `int32_t`: `0` = 11W Motor, `1` = 5.5W Motor, `2` = Motor Controller 55 |
| `installed()` | `bool` — detected on the configured Smart Port |

**There is no `position()`, `velocity()` read-back, `isDone()`, or `isSpinning()`.** The MC55 has no encoder of its own — see [vex-autonomous-concepts.md](vex-autonomous-concepts.md#autonomous-progression-with-motor-controller-55-no-built-in-encoder) for what this means for autonomous.

### Gotchas (specific to this robot)

1. **"Motors feel slow" is almost always the default-50%-velocity gotcha** — call `setVelocity(100, percent)` (or pass velocity directly to `spin()`) before assuming a hardware problem.
2. **Do not physically divide/reduce battery voltage to "fix" speed.** Speed is a software setting (`setVelocity`/`spin` voltage argument); the controller already regulates its ~8V max output via PWM internally regardless of the ~12.8V battery. A resistive divider wastes power as heat and adds unpredictable voltage drop under load — it is not how this platform controls speed.
3. **No `spinFor`/`driveFor`/`isDone`** — don't copy auton code examples written for `vex::motor` or the `drivetrain`/`smartdrive` classes; they won't compile against `motor55`. See [vex-autonomous-concepts.md](vex-autonomous-concepts.md).
4. **Only `coast`/`brake` stopping** — don't write code assuming `hold` works on this controller (it silently isn't supported; it won't error, it just won't hold).
5. Construct `motor55` objects once as globals (e.g. in `robot-config.cpp`), same as any other device — not inside `usercontrol()`'s loop.

### If you've misidentified the controller: Motor 393 / Motor Controller 29 (`vex::motor29`)

If the physical controller turns out to be a **3-wire** box (not a Smart Port device) driving an older **Motor 393**, that's a **different** class — `vex::motor29` — not `motor55`. Quick way to tell: MC55 plugs into a **Smart Port** on the Brain; a Motor Controller 29 plugs into a **3-Wire Port**.

[Motor 393 (Motor Controller 29) API](https://api.vex.com/v5/home/cpp/Motors_and_MotorControllers/motor_393.html)

```cpp
motor29(triport::port &port);                  // e.g. Brain.ThreeWirePort.A
motor29(triport::port &port, bool reverse);

motor29 Motor393_A = motor29(Brain.ThreeWirePort.A);
Motor393_A.spin(forward, 50, percent);          // velocity units here, not voltage
Motor393_A.setVelocity(50, percent);
```

`motor29` has **even less telemetry than `motor55`** — per VEX's own docs, "a Motor 393 does not report position, velocity, current, power, torque, efficiency, or temperature." Only `spin`, `stop`, `setVelocity`, `setReversed` exist. Confirm which controller you actually have before writing drive code — the port type (Smart Port vs. 3-Wire) is the fastest way to check.

---

## Motor (`vex::motor`) — optional, future-upgrade path (V5 Smart Motors)

> **This section does not describe this robot's current drivetrain.** The team uses `vex::motor55` (above), which has no encoder and a different, smaller API surface. Everything below only becomes relevant if the team upgrades to real V5 Smart Motors — keep it in mind as a "what we'd gain" reference, not as code to write today.

[Motor and Motor Group API](https://api.vex.com/v5/home/cpp/Motors_and_Motor_Controllers/motor_and_motor_group.html)

### Constructors

```cpp
motor(int32_t index);
motor(int32_t index, bool reverse);
motor(int32_t index, gearSetting gears);
motor(int32_t index, gearSetting gears, bool reverse);
```

| Parameter | Detail |
|---|---|
| `index` | Smart Port, written `PORTx` (e.g. `PORT1`) |
| `reverse` | `false` (default) = normal; `true` = reversed direction |
| `gears` | `gearSetting` cartridge — see table below. If omitted, the motor auto-detects the installed cartridge. |

**Gear cartridges (`gearSetting`):**

| Enum | Cartridge color | Free speed |
|---|---|---|
| `ratio36_1` | Red | 100 RPM |
| `ratio18_1` | Green | 200 RPM |
| `ratio6_1` | Blue | 600 RPM |

```cpp
motor LeftMotor = motor(PORT1);
motor RightMotor = motor(PORT2, ratio18_1, true);  // green cartridge, reversed
```

By default `forward` spins a motor **counterclockwise**; `reverse` spins **clockwise** (swapped if the motor was constructed with `reverse = true`).

### Actions

| Method | Signature (common overload) | Notes |
|---|---|---|
| `spin` | `void spin(directionType dir)` <br> `void spin(directionType dir, double velocity, velocityUnits units)` <br> `void spin(directionType dir, double voltage, voltageUnits units)` | Spins forever. **Non-waiting.** `isDone`/`isSpinning` don't apply — no target. |
| `spinFor` | `bool spinFor(double rotation, rotationUnits units, double velocity, velocityUnits units_v, bool waitForCompletion = true)` <br> (dir, time, and default-velocity overloads also exist) | Spins a distance or time. **Target-based** — `waitForCompletion` defaults `true`. |
| `spinToPosition` | `bool spinToPosition(double rotation, rotationUnits units, double velocity, velocityUnits units_v, bool waitForCompletion = true)` | Spins to an **absolute** position (not relative — direction depends on current position vs. target). |
| `stop` | `void stop()` <br> `void stop(brakeType mode)` | `coast`, `brake`, or `hold`. |

`rotationUnits`: `deg`/`degrees`, `turns`/`rev`. `velocityUnits`: `percent`/`pct`, `rpm`, `dps`. `timeUnits`: `seconds`/`sec`, `msec`.

```cpp
Motor1.spin(forward);
wait(1, seconds);
Motor1.stop(coast);

Motor1.spinFor(forward, 1, turns, 50, velocityUnits::pct);   // waits by default
Motor1.spinFor(forward, 200, degrees, false);                 // non-waiting
while (!Motor1.isDone()) { wait(10, msec); }
```

### Mutators

| Method | Signature | Notes |
|---|---|---|
| `setPosition` | `void setPosition(double value, rotationUnits units)` | Resets the reference point used by `position()`/`spinToPosition`. |
| `setVelocity` | `void setVelocity(double velocity, velocityUnits units)` | Default velocity used when a movement call doesn't specify one. Starts at 50%. |
| `setStopping` | `void setStopping(brakeType mode)` | Default brake mode for subsequent `stop()` calls. |
| `setMaxTorque` | `void setMaxTorque(double value, percentUnits/torqueUnits/currentUnits units)` | Caps how hard the motor can push. Default 50%. |
| `setTimeout` | `void setTimeout(int32_t time, timeUnits units)` | Bails out of a target-based move (`spinFor`/`spinToPosition`) after this long if the target isn't reached. |

### Getters

| Method | Returns |
|---|---|
| `isDone()` / `isSpinning()` | `bool` — only meaningful after `spinFor`/`spinToPosition` |
| `position(rotationUnits units)` | Current position, `double` |
| `velocity(velocityUnits units)` | Current speed, `double` (signed) |
| `current(currentUnits/percentUnits units = amp)` | 0.0–1.2 A |
| `power(powerUnits units = watt)` | 0.0–22.0 W |
| `torque(torqueUnits units = Nm)` | Nm or InLb |
| `efficiency(percentUnits units = percent)` | 0–100% |
| `temperature(percentUnits/temperatureUnits units = percent)` | Keep below 55°C; motor cuts out near 70°C |
| `voltage(voltageUnits units = volt)` | Supplied voltage |
| `direction()` | `directionType` — `forward` or `reverse` |
| `installed()` | `bool` — connected to the Brain |

### Gotchas

1. **`spin()` is non-waiting; `spinFor`/`spinToPosition` default to waiting** (`waitForCompletion = true`). Mixing these up is the #1 "my auton skipped a step" bug.
2. **`spinToPosition` is absolute**, not relative — a repeated call with the same value does nothing.
3. **Negative `velocity`/voltage values spin opposite `dir`** — don't double-negate direction and sign.
4. Construct motors **once as globals** (or in your device-config file) — not inside `usercontrol()`'s loop.
5. **`setMaxTorque` with `currentUnits` limits by current draw**, not raw torque — useful for lift/arm stall protection.

---

## Motor Group (`vex::motor_group`) — optional, future-upgrade path

> Same caveat as above: `motor_group` only accepts `vex::motor` (Smart Motor) objects — it cannot group `motor55` objects. Not usable with this robot's current drivetrain.

```cpp
motor_group(motor &m1, Args&... m2);  // up to 4 motors total
```

```cpp
motor LeftMotor = motor(PORT1);
motor RightMotor = motor(PORT2);
motor_group DriveMotors = motor_group(LeftMotor, RightMotor);

DriveMotors.spin(forward, 50, percent);
```

Same action/mutator/getter surface as `motor`. **Gotcha:** on a `motor_group`, single-value getters (`position()`, `velocity()`, `power()`, `direction()`, `temperature()`) return the **first motor's** value, not an aggregate — except `current()`, which sums all motors in the group. `count()` returns how many motors are in the group.

---

## Controller (`vex::controller`)

[Controller API](https://api.vex.com/v5/home/cpp/Controller/index.html) · [Axis API](https://api.vex.com/v5/home/cpp/Controller/Axis.html) · [Button API](https://api.vex.com/v5/home/cpp/Controller/Button.html)

### Constructor

```cpp
controller();                       // primary controller
controller(controllerType id);      // controllerType::primary or ::partner — two controllers
```

### Joysticks (`axis`)

| Object | Maps to |
|---|---|
| `Controller.Axis1` | Right joystick, horizontal |
| `Controller.Axis2` | Right joystick, vertical |
| `Controller.Axis3` | Left joystick, vertical |
| `Controller.Axis4` | Left joystick, horizontal |

```cpp
double x = Controller.Axis1.position();   // -100 to 100
Controller.Axis1.changed(myCallback);     // runs myCallback() on axis change
```

### Buttons (`button`)

Available button objects: `ButtonA/B/X/Y`, `ButtonUp/Down/Left/Right`, `ButtonL1/L2/R1/R2`.

```cpp
if (Controller.ButtonA.pressing()) { /* held right now */ }
Controller.ButtonR1.pressed(myCallback);   // runs once, on rising edge
Controller.ButtonR1.released(myCallback);
```

### Screen

```cpp
Controller.Screen.clearScreen();
Controller.Screen.setCursor(1, 0);          // line 0-2, column
Controller.Screen.print("Auton: %s", name);
```

Controller screen/rumble updates are **slow** (throttle below ~10 ms USB / ~50 ms VEXnet) — don't call every loop iteration.

### Rumble

```cpp
Controller.rumble(".-.");   // '.' = short, '-' = long pulse
```

---

## Inertial Sensor (`vex::inertial`)

[Inertial API](https://api.vex.com/v5/home/cpp/Smart_Port_Devices/Inertial_Sensor.html)

### Constructor

```cpp
inertial(int32_t index, turnType dir = turnType::right);
```

`dir` sets which turn direction reports as **positive** heading change (`turnType::right` = clockwise, default; `turnType::left`).

### Calibration (call in `pre_auton()`)

```cpp
Inertial.calibrate();
while (Inertial.isCalibrating()) {
  wait(50, msec);
}
```

**Robot must be completely still during calibration** — vibration or motion causes drift for the rest of the match.

### Readings

| Method | Returns | Use |
|---|---|---|
| `heading(rotationUnits units = degrees)` | 0 – 359.99° (bounded) | Turn-to-heading auton |
| `rotation(rotationUnits units = degrees)` | Unbounded, signed | Cumulative-turn PID |
| `acceleration(axisType axis)` | Linear accel, g | Collision/disturbance detection |
| `gyroRate(axisType axis)` | deg/s | Rate feedback |
| `orientation(axisType axis, rotationUnits units)` | Roll/pitch/yaw | Tilt detection |

### Reset / set

| Method | Effect |
|---|---|
| `resetHeading()` | Current heading → 0 |
| `resetRotation()` | Cumulative rotation → 0 |
| `setHeading(double value, rotationUnits units)` | Force heading to a value |
| `setRotation(double value, rotationUnits units)` | Force rotation to a value |
| `setTurnType(turnType dir)` / `getTurnType()` | Change/read which direction is positive |

### Gotchas

- **`heading()` wraps 0–359.99°; `rotation()` accumulates without bound** — use `rotation()` for PID on total turn amount (no wraparound math needed), `heading()` for "face this compass direction."
- Mount flat and secure; a loose IMU reads noisy headings.
- `collision(callback)` and `changed(callback)` exist for event-driven code, but polling in your auton/opcontrol loop is simpler for a first season.

---

## Rotation Sensor (`vex::rotation`)

[Rotation API](https://api.vex.com/v5/home/cpp/Smart_Port_Devices/Rotation_Sensor.html)

### Constructor

```cpp
rotation(int32_t index, bool reverse = false);
```

### Methods

| Method | Returns |
|---|---|
| `angle(rotationUnits units = degrees)` | 0 – 359.99° (bounded) |
| `position(rotationUnits units)` | Cumulative, signed |
| `velocity(velocityUnits units)` | `dps` or `rpm` |
| `setPosition(double value, rotationUnits units)` | Resets the reference point for `position()` |

**Use case:** tracking wheels for odometry — higher resolution than motor encoders and no drivetrain gearbox backlash, since the sensor rides directly on the tracking wheel's axle.

---

## Distance Sensor (`vex::distance`)

[Distance API](https://api.vex.com/v5/home/cpp/Smart_Port_Devices/Distance_Sensor.html)

### Constructor

```cpp
distance(int32_t index);
```

### Methods

| Method | Returns |
|---|---|
| `objectDistance(distanceUnits units)` | `mm` (20–2000) or `in` |
| `objectSize()` | `sizeType`: `none`, `small`, `medium`, `large` |
| `objectVelocity()` | m/s, signed (closing/opening) |
| `isObjectDetected()` | `bool` |
| `installed()` | `bool` |

**Use case:** wall alignment, goal detection, intake indexing. Valid range is 20 mm–2000 mm — gate on `isObjectDetected()` before trusting `objectDistance()`.

---

## Optical Sensor (`vex::optical`)

[Optical API](https://api.vex.com/v5/home/cpp/Smart_Port_Devices/Optical_Sensor.html)

### Constructor

```cpp
optical(int32_t index);
```

### Methods

| Method | Returns |
|---|---|
| `hue()` | 0 – 359.99 |
| `brightness(bool bRaw = false)` | 0 – 100% (or raw if `true`) |
| `color()` | `vex::color` — nearest predefined color match |
| `isNearObject()` | `bool` |
| `setLight(ledState state)` | LED `on`/`off` |
| `setLightPower(double power, percentUnits units)` | LED brightness 0–100% |
| `objectDetected(callback)` / `objectLost(callback)` | Event callbacks |

**Use case:** color sorting (alliance game pieces), line following. Keep the LED on (`setLight(ledState::on)`) for consistent hue readings — ambient light shifts hue otherwise.

---

## Pneumatics / 3-Wire (ADI)

VEX's docs call the physical ports **3-Wire ports** (what PROS calls "ADI"). Two ways to drive a solenoid, both constructed by hand (neither is available via a graphical Devices wizard in VS Code today):

### Preferred: `digital_out` (3-Wire Digital Out)

[Digital Out API](https://api.vex.com/v5/home/cpp/3-wire/DigitalOut.html)

```cpp
brain Brain;
digital_out PistonA = digital_out(Brain.ThreeWirePort.A);

PistonA.set(true);   // energize — typically extend, confirm wiring
PistonA.set(false);  // de-energize — typically retract
```

### Alternative: `pneumatics`

[Pneumatics API](https://api.vex.com/v5/home/cpp/3-Wire_Devices/Pneumatics.html)

```cpp
pneumatics PistonA = pneumatics(Brain.ThreeWirePort.A);

PistonA.open();   // extend — air flows into the cylinder
PistonA.close();  // retract
```

| Parameter | Detail |
|---|---|
| `Brain.ThreeWirePort.X` | 3-Wire port letter `A`–`H` on the Brain, or `ExpanderName.X` on a 3-Wire Expander |

**Gotcha:** confirm with mechanical whether you have a **single-solenoid** (spring return, one 3-wire port) or **double-solenoid** (two 3-wire ports, one per direction) setup before writing extend/retract logic — the wiring determines whether "energized" means extend or retract.

---

## 3-Wire Servo (`vex::servo`)

A position servo on a **lettered 3-Wire Port**, not a Smart Port and not `motor55`. It moves to an angle and holds. It does not spin continuously, and the current C++ page lists no speed command and no position read-back. Longer teaching notes: [servo.md](servo.md).

[Servo Motor API](https://api.vex.com/v5/home/cpp/Motors_and_MotorControllers/servo_motor.html) · degree travel also stated on the [Blocks servo page](https://api.vex.com/v5/home/blocks/motion/servo_motor.html)

### Constructor

```cpp
servo(triport::port &port);
```

```cpp
servo TestServo = servo(Brain.ThreeWirePort.A);  // letters A–H
```

The Brain (or a 3-Wire Expander) must already exist. On an expander the port is `ExpanderName.A`.

### setPosition

The only member function the C++ page documents:

```cpp
void setPosition(int32_t value, percentUnits units);   // percent: 0–100%
void setPosition(double value, rotationUnits units);   // degrees or turns
```

| | What the docs say |
|---|---|
| Percent | Valid range **0–100%** |
| Degrees | Absolute position. The Blocks page for this device says travel is **0–100 degrees**, and the horn holds that position. C++ examples use 45.5° and 90°. |
| Center | Both pages: the servo **centers itself at 50 degrees** when a project starts |
| Read-back | Not documented. Print the angle you commanded. |

```cpp
TestServo.setPosition(50, degrees);  // center
TestServo.setPosition(0, degrees);   // one end of travel
```

`setPosition` does not take a speed and does not wait. To move slowly, command a new angle a little at a time from your driver loop.

---

## Drivetrain built-ins (`vex::drivetrain` / `vex::smartdrive`) — not usable with this robot's current drivetrain

> **Not available for this robot today.** Both constructors below require `motor &`/`motor_group &` (V5 Smart Motor) arguments — the official API explicitly types the left/right parameters as `motor`/`motor_group`, and `motor55` is neither. With MC55-driven motors, there is no built-in `driveFor`/`turnFor`/`turnToHeading` to reach for. Kept here as a future-upgrade reference; see [vex-autonomous-concepts.md](vex-autonomous-concepts.md#autonomous-progression-with-motor-controller-55-no-built-in-encoder) for what to actually use instead.

[Drivetrain API](https://api.vex.com/v5/home/cpp/Drivetrain/index.html) · [Smartdrive API](https://api.vex.com/v5/home/cpp/Drivetrain/smartdrive.html)

| Method | Available on | Notes |
|---|---|---|
| `drive(directionType dir, double velocity, velocityUnits units)` | `drivetrain`, `smartdrive` | Open-loop, forever, non-waiting |
| `driveFor(directionType dir, double distance, distanceUnits units, bool waitForCompletion = true)` | both | Closed-loop distance move |
| `turnFor(turnType dir, double angle, rotationUnits units, bool waitForCompletion = true)` | both | Closed-loop turn (no heading sensor needed) |
| `turnToHeading(double angle, rotationUnits units, bool waitForCompletion = true)` | `smartdrive` only | Needs the sensor passed to the constructor; turns the shortest way to an **absolute** heading |
| `setDriveVelocity` / `setTurnVelocity` | both | Default speeds for the above |
| `stop(brakeType mode)` / `setStopping` / `setTimeout` | both | Same semantics as `motor` |

See [vex-autonomous-concepts.md § Autonomous progression with Motor Controller 55](vex-autonomous-concepts.md#autonomous-progression-with-motor-controller-55-no-built-in-encoder) for what this robot actually uses instead.

---

## Utility functions you'll use constantly

| Function | Purpose |
|---|---|
| `wait(double time, timeUnits units)` | Yield for a time — **required** in every `usercontrol`/auton polling loop |
| `vexDelay(uint32_t ms)` | Lower-level delay (rare — prefer `wait()`) |
| `printf(...)` | Debug output — see [vex-setup.md § Debugging](vex-setup.md#debugging-printf-and-the-interactive-terminal) |
| `Brain.Screen.print(...)` / `Brain.Screen.clearScreen()` | Draw to the V5 Brain's built-in screen |

---

## Units quick-reference

| Quantity | Typical VEX SDK unit | Typical use |
|---|---|---|
| Rotation Sensor position | `degrees` or `turns` | Auton distance (with wheel math) — **required on this robot**, since `motor55` has no position of its own |
| Rotation Sensor velocity | `rpm` or `dps` | Speed read-back, PID feedback |
| IMU heading | `degrees`, 0–359.99 | Turn PID setpoint |
| IMU rotation | `degrees`, unbounded | Cumulative-turn PID |
| Distance sensor | `mm` (20–2000) or `in` | Wall distance |
| Optical hue | 0–359.99 | Color sorting |
| 3-wire servo angle | `degrees`, **0–100**, center **50** (or `percent` 0–100) | `vex::servo` `setPosition` — no read-back. See [servo section](#3-wire-servo-vexservo) |

### Wheel distance math

```
distance = (rotation_sensor_degrees / 360) × external_gear_ratio × wheel_circumference
```

**On this robot, `rotation_sensor_degrees` must come from an external Rotation Sensor mounted on the axle/wheel** — `motor55` has no encoder to read, so there is no motor-degrees source for this formula unless/until a Rotation Sensor is added. Get `external_gear_ratio` and `wheel_circumference` from mechanical — see [vex-learning-path.md § Working with the mechanical subteam](vex-learning-path.md#working-with-the-mechanical-subteam). If the Rotation Sensor rides directly on the wheel axle with no external gearing, `external_gear_ratio = 1`.

---

← [Setup](vex-setup.md) · [Autonomous concepts](vex-autonomous-concepts.md) · [Learning path](vex-learning-path.md)
