# Autonomous Concepts — From Timers to Odometry, in VEX SDK Terms

Driver control, autonomous progression, and the question of when (if ever) you need a community library. Game-agnostic; timed for a **15-second** VRC autonomous period (Override 2026–2027). Written for the **official VEX C++ SDK** (`vex::` namespace) — not PROS.

> **Hardware note (Oct 8, 2026, evening):** this robot's drivetrain is **DC motors through Motor Controller 55 (`vex::motor55`)**, not V5 Smart Motors. **`motor55` has no built-in encoder** — no `position()`, no `spinFor`, no `isDone`, and the SDK's built-in `drivetrain`/`smartdrive` closed-loop class (`driveFor`/`turnFor`/`turnToHeading`) isn't usable either, since it requires `motor`/`motor_group` objects. The ladder below is corrected for that reality. See [project-context.md § Hardware](project-context.md#hardware-corrected-oct-8-2026-evening--authoritative) and [vex-api-cheatsheet.md § Primary Drive API](vex-api-cheatsheet.md#primary-drive-api-motor-controller-55-vexmotor55).

← [Learning path](vex-learning-path.md) · [API cheatsheet](vex-api-cheatsheet.md) · [Setup](vex-setup.md)

---

## Driver control

### Tank drive

Left stick Y → left side motor; right stick Y → right side motor. Best for skid-steer (standard VRC chassis) — and this robot only has one motor per side, so there's no grouping to worry about.

**`motor55` has no `spin(dir, velocity, percentUnits)` overload** — only `spin(dir)` (uses the currently configured velocity) and `spin(dir, voltage, voltageUnits)`. Set speed with `setVelocity()` first, then `spin(dir)`:

```cpp
void usercontrol(void) {
  while (true) {
    double left  = Controller1.Axis3.position(percent);   // left stick, vertical, -100..100
    double right = Controller1.Axis2.position(percent);   // right stick, vertical, -100..100

    LeftDrive.setVelocity(fabs(left), percent);
    LeftDrive.spin(left >= 0 ? forward : reverse);

    RightDrive.setVelocity(fabs(right), percent);
    RightDrive.spin(right >= 0 ? forward : reverse);

    wait(20, msec);
  }
}
```

### Arcade drive

One stick throttle + one stick turn. Common mapping: left Y = forward, right X = turn.

```cpp
double throttle = Controller1.Axis3.position(percent);
double turn     = Controller1.Axis1.position(percent);     // right stick, horizontal
double left  = throttle + turn;
double right = throttle - turn;
// clamp left/right to ±100, then apply the same setVelocity()+spin() pattern as tank drive above
```

Arcade feels faster to learn but can be harder for precise alignment — many college teams still use tank or a hybrid.

### Deadband

Joystick hardware doesn't return exactly 0 at rest. Ignore small values:

```cpp
if (fabs(value) < 5) value = 0;
```

Typical deadband: **5–10** (out of ±100, since `Axis*.position()` returns percent). Tune until the robot doesn't creep at rest.

### Drive curves

Linear stick → linear power makes fine control at low speed difficult. Apply a curve before passing to `setVelocity()`:

| Curve | Formula (concept) | Feel |
|---|---|---|
| Linear | `power = stick` | Aggressive, hard to align |
| Cubic | `power = stick³ / 100²` | Smooth low-end, full power at max |
| Squared | `power = sign(stick) * stick² / 100` | Middle ground |

Compute in `double`, clamp to [-100, 100], then pass the magnitude to `setVelocity()` and the sign to pick `forward`/`reverse` for `spin()`.

### Myth check: don't physically divide battery voltage

If a motor feels underpowered, the fix is **always software, never a physical voltage divider**: call `setVelocity(100, percent)` (every project starts at a default 50%) or raise the voltage argument toward `motor55`'s ±8V ceiling. A resistive divider wastes power as heat and adds unpredictable sag under load — it's not how speed control works here. See [vex-api-cheatsheet.md § Gotchas](vex-api-cheatsheet.md#gotchas-specific-to-this-robot).

---

## Autonomous progression with Motor Controller 55 (no built-in encoder)

**The standard VEX SDK ladder (time → encoder → PID → IMU → odometry) assumes a Smart Motor's built-in encoder.** `motor55` doesn't have one, so step 2 of that ladder — "switch from timing to the motor's own position reading" — **isn't available on this robot without adding hardware.** Here's the corrected progression:

| Stage | Technique | Needs |
|---|---|---|
| 1 | Time-based (`spin` + `wait` + `stop`) | Nothing extra — works today, this is the practical default |
| 1.5 | Time-based + current-draw stall check | Nothing extra — `current()`/`voltage()` are the only telemetry `motor55` has |
| **Upgrade path** → 2 | Distance-based, using an **external Rotation Sensor** mounted on a wheel/axle | A Rotation Sensor (separate smart-port device) — not provided by `motor55` itself |
| 3–4 | Hand-rolled PID on distance (via the Rotation Sensor) and/or heading (via Inertial) | Rotation Sensor and/or Inertial Sensor; **no `drivetrain`/`smartdrive` built-in available** |
| 5–6 | Odometry / community libraries | Out of scope this arc — see [below](#odometry-and-community-libraries) regardless of motor type |

### Stage 1: Time-based (the practical default for this robot)

```cpp
LeftDrive.setVelocity(80, percent);
RightDrive.setVelocity(80, percent);
LeftDrive.spin(forward);
RightDrive.spin(forward);
wait(500, msec);
LeftDrive.stop();
RightDrive.stop();
```

Battery voltage, friction, and floor surface change every run — timing drifts. **This is still the realistic baseline for this robot** unless/until a Rotation Sensor is added, not just a "proof of life" step to graduate past quickly. Mitigate drift by:

- Running the same test 5–10× and tuning the `wait()` duration to the median result, not the first run
- Testing at the battery voltage you'll actually compete at (don't tune on a fresh battery, run at a tired one)
- Keeping each move short — error compounds the longer a move runs open-loop

For a 15-second auton (Override and most VRC seasons), budget ~2–3 s per major move including settle time. Chain 2–3 time-based moves max before accumulated drift makes the routine unreliable — this is a tighter budget than a Smart-Motor-based team gets, because there's no closed-loop correction available yet.

### Stage 1.5: Add a stall/current check (optional, no new hardware)

`motor55` does expose `current()`. You can use a current spike as a crude "did we hit something" signal, without any position feedback:

```cpp
LeftDrive.spin(forward);
RightDrive.spin(forward);
wait(100, msec);  // let current settle after start
while (LeftDrive.current(amp) < 1.0 && RightDrive.current(amp) < 1.0) {
  wait(20, msec);
}
LeftDrive.stop();
RightDrive.stop();
```

This isn't a distance measurement — it's a safety stop (e.g. "stop driving into a wall"), not a replacement for the Rotation Sensor upgrade below.

### Upgrade path → Stage 2: Add an external Rotation Sensor for real distance feedback

Because `motor55` has no encoder, the only way to get real distance feedback is a **separate Rotation Sensor** (`vex::rotation`) mounted on a drive axle or an idler/tracking wheel. This is a hardware addition the team would need to make — it isn't optional config, it's a part that has to be bought/mounted and wired to a Smart Port.

```cpp
rotation DriveRotation = rotation(PORT3);  // mounted on the drive axle

DriveRotation.setPosition(0, degrees);
LeftDrive.setVelocity(60, percent);
RightDrive.setVelocity(60, percent);
LeftDrive.spin(forward);
RightDrive.spin(forward);

while (fabs(DriveRotation.position(degrees)) < 900) {   // target distance in encoder degrees
  wait(10, msec);
}
LeftDrive.stop();
RightDrive.stop();
```

**Convert degrees to distance:** see [vex-api-cheatsheet.md § Wheel distance math](vex-api-cheatsheet.md#wheel-distance-math) — on this robot, the "motor degrees" input to that formula must come from the Rotation Sensor, not from `motor55` (which has nothing to read).

If the team decides not to add a Rotation Sensor this arc, **Stage 1 (time-based) is the ceiling for autonomous** until that hardware upgrade happens — plan the Oct 12 checkpoint around that.

---

### Stage 3 & 4: PID and IMU heading (both require the Stage 2 hardware upgrade)

**Minimal PID loop (distance example, using an external Rotation Sensor — not the motor):**

```cpp
double error = target_degrees - DriveRotation.position(degrees);
integral += error;
double derivative = error - last_error;
double output = Kp * error + Ki * integral + Kd * derivative;

double voltage = clamp(output, -8.0, 8.0);  // motor55's spin-at-voltage range is ±8V, not ±100%
LeftDrive.spin(voltage >= 0 ? forward : reverse, fabs(voltage), volt);
```

**Tuning order:** set `Ki = Kd = 0`. Increase `Kp` until oscillation, then back off ~20%. Add a small `Kd` to damp the oscillation. Add a tiny `Ki` only if steady-state error remains. Note the output is clamped to `motor55`'s **±8V** `spin()` range, not the ±100% percent range you'd use with a Smart Motor — mixing those up will either do nothing (percent values way under 8) or get clamped silently (percent values treated as volts).

**IMU heading correction while driving straight** (requires an Inertial Sensor — a separate smart-port device, independent of the drive motor question):

```cpp
double heading_error = target_heading - Inertial.heading(degrees);
// wrap heading_error to -180..180 before using it
double turn_correction = Kp_heading * heading_error;

LeftDrive.setVelocity(clamp(forward_power - turn_correction, 0, 100), percent);
LeftDrive.spin(forward);
RightDrive.setVelocity(clamp(forward_power + turn_correction, 0, 100), percent);
RightDrive.spin(forward);
```

**Turn to absolute heading — hand-rolled only; there is no `turnToHeading()` for this drivetrain:**

```cpp
double target = 90.0;
while (fabs(wrap(target - Inertial.heading(degrees))) > 2.0) {
  double error = wrap(target - Inertial.heading(degrees));
  double power = clamp(Kp_turn * error, -80, 80);
  LeftDrive.setVelocity(fabs(power), percent);
  LeftDrive.spin(power >= 0 ? forward : reverse);
  RightDrive.setVelocity(fabs(power), percent);
  RightDrive.spin(power >= 0 ? reverse : forward);
  wait(10, msec);
}
LeftDrive.stop();
RightDrive.stop();
```

Call `Inertial.calibrate()` in `pre_auton()` (stationary!) before any auton that uses heading. See [vex-api-cheatsheet.md § Inertial Sensor](vex-api-cheatsheet.md#inertial-sensor-vexinertial).

> **Reminder:** `drivetrain`/`smartdrive` and their `driveFor`/`turnFor`/`turnToHeading` methods are **not usable with `motor55`** — they require `motor`/`motor_group` constructor arguments. There is no shortcut here; everything above is hand-rolled by necessity, not by choice. See [vex-api-cheatsheet.md § Drivetrain built-ins](vex-api-cheatsheet.md#drivetrain-built-ins-vexdrivetrain--vexsmartdrive--not-usable-with-this-robots-current-drivetrain).

---

### Odometry and community libraries

**Odometry** estimates robot pose `(x, y, θ)` on the field by integrating tracking-wheel displacement with IMU heading. On this robot, that would mean two parallel Rotation-Sensor-based tracking wheels + an Inertial Sensor — i.e. it depends on the same Stage 2 hardware upgrade above, plus more of it.

**Important correction vs. what you may read online:** the two most commonly recommended VEX community motion libraries — **[LemLib](https://lemlib.readthedocs.io/)** and **[EZ-Template](https://ez-robotics.github.io/EZ-Template/)** — are both **PROS-only**. They install through the PROS VS Code extension and PROS CLI/terminal and depend on PROS's build system and `pros::Motor`'s built-in encoder. **Neither runs on the official VEX C++ SDK, and both assume a Smart-Motor-equivalent encoder that this robot's `motor55` doesn't have anyway.** The same is true of OkapiLib. If a tutorial shows `#include "lemlib/api.hpp"`, that project is a PROS project with Smart Motors — not this stack, and not this hardware.

**What that means for this team, concretely:**

| Option | Tradeoff |
|---|---|
| Stay on `motor55`, time-based auton only | Zero new hardware; ceiling is "drive off the line and stop," no real PID or heading correction |
| Add a Rotation Sensor (+ optionally an Inertial Sensor) and hand-roll PID/heading on top of `motor55` | One or two sensor purchases + mounting; real closed-loop auton without changing the drive motors or the SDK |
| Upgrade the drivetrain to actual V5 Smart Motors | Bigger hardware/budget change; unlocks `spinFor`, `driveFor`, `turnFor`, `turnToHeading`, and eventually odometry/third-party libraries *if also switching to PROS* |
| Switch the whole project to PROS to gain LemLib/EZ-Template | A full stack change, not a small addition — explicitly out of scope per the stack decision in [project-context.md](project-context.md); revisit only as a deliberate, team-wide decision for a *future* season |

**For this team, in this 6-week arc:** don't adopt odometry or a community library, and don't plan on it. Get **one reliable time-based auton** working by Oct 12 ("drive off the line and stop" is enough to pass the checkpoint). Use Weeks 4–5 to decide, with mechanical, whether adding a Rotation Sensor is worth it for a second, more accurate auton — that decision is now a **hardware** decision as much as a software one.

---

## Auton selector pattern

**Goal:** pick among multiple routines at competition without recompiling. This pattern is unaffected by the motor-type correction — it works the same with `motor55` as it would with Smart Motors.

### Structure

```
include/
  auton.h            // declares auton routine functions
src/
  auton/
    auton_left.cpp    // void auton_left();
    auton_right.cpp
    auton_skills.cpp
  main.cpp
```

### `main.cpp` integration

```cpp
int selected_auton = 0;

void pre_auton(void) {
  Inertial.calibrate();   // omit if no Inertial Sensor is installed yet
  while (Inertial.isCalibrating()) { wait(50, msec); }

  while (!Competition.isEnabled()) {
    if (Controller1.ButtonUp.pressing())   selected_auton = (selected_auton + 1) % 3;
    Brain.Screen.clearScreen();
    Brain.Screen.print("Auton: %d", selected_auton);
    wait(50, msec);
  }
}

void autonomous(void) {
  switch (selected_auton) {
    case 0: auton_left(); break;
    case 1: auton_right(); break;
    case 2: auton_skills(); break;
  }
}
```

### Selection UI options

| Method | Pros | Cons |
|---|---|---|
| Controller button + Brain screen text | Works on field, no extra hardware | Must select before enable; needs a `pre_auton` polling loop (shown above) |
| Brain screen touch | Visual | Hard to see/reach mid-field |
| Hardcoded constant, recompile per routine | Simple | Requires a laptop at the field for every change |

**Practice pattern:** print the selected routine name on `Brain.Screen` during the `pre_auton` wait loop; confirm visually before the match starts.

---

## Code structure for easy auton swaps

1. **One routine per file** — `auton_left.cpp` only knows left-side logic.
2. **Shared primitives** — `drive_time(double power, double seconds)` today; `drive_inches(double)`/`turn_to_heading(double)` once a Rotation/Inertial Sensor is added — live in your drivetrain file, not copy-pasted into every auton.
3. **No magic numbers in autons** — constants in `robot-config.h` or a dedicated constants header (`AUTON_DRIVE_SPEED`, `AUTON_MOVE_MS`).
4. **Subsystem methods** — `intake.deploy()`, `lift.toHeight(LOW)` instead of raw motor calls inside auton code.
5. **Test hook** — map a controller button in `usercontrol()` to call `auton_left()` for practice without field control.

---

## Tuning and testing workflow

### Before each test session

- [ ] Battery above 12.5 V (or note voltage for comparison) — note the team's battery is ~12.8V nominal; test near the voltage you'll actually compete at, not just fresh off the charger
- [ ] Interactive Terminal connected and "Enable User Terminal" on (see [vex-setup.md](vex-setup.md#debugging-printf-and-the-interactive-terminal))
- [ ] Wheels on the same surface as competition (tile vs. carpet)
- [ ] Auton routine name logged at start (`printf` or `Brain.Screen.print`)

### Per-move tuning (time-based, this robot's current ceiling)

1. **Hypothesis** — "Overshooting by ~6 inches"
2. **Measure** — mark tape on the floor, measure where the robot actually stops
3. **Change one variable** — `wait()` duration or `setVelocity()` value, one at a time
4. **Run 5 times** — record success rate and stopping position
5. **Log** — spreadsheet: date, battery voltage, surface, result

Once a Rotation Sensor is added, this becomes the same workflow but tuning `Kp`/`Ki`/`Kd` or target encoder degrees instead of a `wait()` duration.

### Match simulation

- Run auton from a competition switch if available — radio latency differs from USB-only testing.
- Run back-to-back auton + driver control without a power cycle (catches `pre_auton()` assumptions that only hold on a cold boot).
- Practice the **15-second cutoff** — use a phone timer; know what happens if auton doesn't finish (mechanisms should end in a safe state).

### When something fails at an event

1. Read the Interactive Terminal log from the last run
2. Check the port map in `robot-config.cpp` vs. the physical robot (most common failure)
3. Check battery voltage — time-based auton is especially sensitive to battery sag since there's no closed-loop correction yet
4. Fall back to a simpler auton if needed — a reliable "drive off the line" beats a flaky multi-step routine

---

## Common beginner mistakes

| Mistake | Symptom | Fix |
|---|---|---|
| No `wait()` in a `usercontrol`/polling loop | Brain freeze, disconnect, or starved tasks | Add `wait(20, msec)` minimum per loop iteration |
| Reconstructing `motor55`/`inertial` objects inside `usercontrol()` | Jittery config, memory churn | Construct devices once as globals (`robot-config.cpp`) |
| Calling `spin(dir, velocity, percentUnits)` on `motor55` | Compile error — no such overload | Use `setVelocity(velocity, percent)` then `spin(dir)`, or `spin(dir, voltage, volt)` with voltage in **−8..8** |
| Treating a PID output as a percent and passing it to `spin(dir, output, volt)` | Motor barely moves (an 80% "percent" value is way outside the ±8V range) or output gets silently misinterpreted | Clamp PID output to **±8V**, not ±100%, before calling `motor55.spin(dir, voltage, volt)` |
| Trying to physically divide/reduce battery voltage to fix "slow" motors | Wasted power, unpredictable voltage sag, motors still default to 50% speed | `setVelocity(100, percent)` in software — see [Driver control § Myth check](#myth-check-dont-physically-divide-battery-voltage) |
| Copying `spinFor`/`driveFor`/`turnToHeading` example code | Compile error — those methods don't exist on `motor55`, and `drivetrain`/`smartdrive` can't be built from `motor55` objects | Use the time-based (or Rotation-Sensor-based) patterns in this doc instead |
| `printf` without `\n` | "Nothing prints," but code is running fine | Add `\n` or `fflush(stdout)` — see [vex-setup.md](vex-setup.md#printf-buffering-gotcha-the-1-nothing-prints-bug) |
| Time-based auton tuned only at the shop | Works at shop, fails at the event (different friction/battery) | Re-tune on the actual competition surface and a realistically-depleted battery before Oct 12 |
| Ignoring IMU calibration (if installed) | Heading drifts 10°+ per match | `Inertial.calibrate()` while stationary, every program start |
| Pneumatics wiring assumption wrong | Piston extends on program start when you expected retracted | Confirm single- vs. double-solenoid wiring with mechanical; test `digital_out`/`pneumatics` state at boot |
| Auton over 15 s | Cut off mid-routine | Time each segment; trim the lowest-value action first |
| Giant `main.cpp` | Unmergeable code, merge conflicts across contributors | Split subsystems into their own `.h`/`.cpp` pairs |
| No version control | "It worked yesterday" | Git commit per working state |

---

## Override-specific timing reminder (optional)

Override 2026–2027 awards an **Autonomous Bonus** and an **Autonomous Win Point** for assigned tasks in the 15 s period. Design autons backward from the score sheet:

1. List required actions and time each (generous estimates) — remember this robot's auton moves are time-based, so "time each" is literally the control variable, not just a budget check.
2. Cut lowest-value actions first if over 15 s.
3. Keep a **safe auton** (preload only, or "drive off the line and stop") and an **aggressive auton** (full route) selectable — see [Auton selector pattern](#auton-selector-pattern) above.

[Override game manual (PDF)](https://content.vexrobotics.com/docs/2026-2027/override/files/v5rc-override-1.0.pdf)

---

← [API cheatsheet](vex-api-cheatsheet.md) · [Learning path](vex-learning-path.md) · [Setup](vex-setup.md)
