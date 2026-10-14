# VEX V5 Competition Starter (VEX Robotics VS Code Extension)

A minimal, heavily-commented VEX **V5** competition template in **C++** for
the official **VEX Robotics VS Code extension**.

> **This is NOT a PROS project.** It does not use the PROS CLI, `pros.h`,
> `okapi`, or the PROS build system. It is built for Microsoft Visual
> Studio Code's VEX Robotics extension (`VEXRobotics.vexcode` on the VS
> Code Marketplace), VEX's own officially supported coding environment
> that replaced VEXcode Pro V5.

> **Drivetrain hardware: two legacy 2-wire DC motors, each driven through
> a VEX Motor Controller 55 (`vex::motor55`) -- NOT V5 Smart Motors
> (`vex::motor`).** That distinction shapes most of this template; see
> "Why this hardware is different" below before assuming anything you've
> read about Smart Motor drivetrains applies here.

If your team is looking for the PROS version of this repo instead, see the
`cursor/pros4-starter-32de` branch -- that is a separate, unrelated stack.

## Why this hardware is different

A V5 Smart Motor (`vex::motor`) has a swappable gear cartridge and a
built-in encoder, so it can report its own velocity and rotation. A
Motor Controller 55 (`vex::motor55`) drives a plain 2-wire DC motor --
there is **no gear cartridge setting** and **no built-in encoder**. The
Brain talks to the MC55 over a Smart Port, but the DC motor on the other
end gives back nothing about how far or how fast it has actually turned.

That has two consequences that run through this whole template:

1. **Driver control is voltage-based, not velocity-based.** `motor55`
   has no internal speed-control loop, so there's no "spin at X%
   velocity" the way a Smart Motor has. What you command is a voltage,
   and a DC motor's speed is roughly proportional to that voltage --
   this template treats "percent power" as "percent of our configured
   max voltage" and converts to volts right before each `spin()` call.
2. **Autonomous is time-based by default, not distance-based.** Without
   an encoder, there's no number to feed a PID loop for "how far have I
   gone." The example autonomous routines drive at a fixed power for a
   measured number of seconds instead -- see `include/config.h` and
   `include/auton-helpers.h` for exactly where encoder-style PID *would*
   plug back in if you add optional external sensors (a Rotation Sensor
   per side, an Inertial Sensor for heading). Those sensors are
   completely independent hardware from the MC55/DC motors -- the
   helpers for them are written and ready, just switched off by default
   with a one-line flag in `config.h` until you actually wire them up.

## What's in here

```
.vscode/              VEX extension project files (see "Verification" below)
include/
  vex.h               VEXcode master header (stub -- see note in the file)
  robot-config.h       extern declarations for all hardware objects
  config.h              <-- the ONE file to edit: ports, reversals, max
                           voltage, deadband, drive mode, optional-sensor
                           switches + PID gains, and the servo port/travel, servo 3-wire port and
                           servo travel/jog/sweep numbers
  drive-control.h      tank/arcade driver control (voltage-based)
  auton-helpers.h       PID class + time-based drive helpers (and
                           optional sensor-feedback helpers, gated off by
                           default)
  autons.h             example autonomous routines + selector
  servo-tests.h        3-wire servo button tests (see docs/servo.md)
src/
  main.cpp             competition callbacks (pre_auton/autonomous/usercontrol)
  robot-config.cpp     builds the hardware objects using config.h's settings
  drive-control.cpp
  auton-helpers.cpp
  autons.cpp
  servo-tests.cpp      servo center / jog / sweep, called from usercontrol
docs/servo.md           what a 3-wire servo is, and the button test
makefile, vex/          VEXcode build system (see "Verification" below)
```

**Only `include/config.h` should need regular edits** for a normal
season: ports, motor reversals, max drive voltage, joystick deadband,
drive mode, the optional-sensor on/off switches, and all PID gains live
there, with comments explaining the *robotics* concept behind each
number -- not just C++ syntax.

## Install the VEX Robotics VS Code extension

1. Install [Visual Studio Code](https://code.visualstudio.com/) (1.66+).
2. Open VS Code, go to the Extensions view (`Ctrl+Shift+X` /
   `Cmd+Shift+X`), search for **"VEX Robotics"**, and install it
   (publisher: VEX Robotics, extension id `VEXRobotics.vexcode`). Install
   the bundled **C/C++** extension (`ms-vscode.cpptools`) if it isn't
   installed automatically -- `.vscode/extensions.json` in this repo will
   prompt you to.
3. A VEX icon appears in the Activity Bar on the left once installed.

## Open this project

1. In VS Code, use **File > Open Folder...** and select this repository's
   root folder (the one containing `src/`, `include/`, and `makefile`).
2. The extension detects it as a valid V5 C++ project because of
   `.vscode/vex_project_settings.json`. If the VEX build/download buttons
   don't appear in the toolbar, open `vex_project_settings.json`, right
   click it, and choose "VEX Project Settings" to let the extension
   re-validate the project (see the Quick notes below if problems persist).
3. The first time you open it, the extension will likely rewrite
   `.vscode/c_cpp_properties.json` and `.vscode/settings.json` to point at
   wherever it installed the V5 SDK/toolchain on your machine -- that's
   expected and fine to let happen (and to commit afterward).

## Build and download to the Brain

1. Connect the V5 Brain to your computer with a USB cable (or make sure
   it's on the same network for wireless download, if your Brain
   supports it).
2. Use the VEX sidebar's **Build** button (or the Command Palette:
   "VEX: Build Current Project") to compile.
3. Use **Download** (or "VEX: Download Current Project") to send the
   compiled program to the Brain. Pick a program slot if asked.
4. On the Brain, select the program from its menu and run it (or let the
   competition switch / field control start it automatically).

This build process is identical regardless of drive motor type -- the
Brain, the Smart Ports, and the VEX build toolchain don't care whether a
Smart Port has a Smart Motor or an MC55 plugged into it.

## Editing ports/wiring for your robot

Open `include/config.h`. Everything you'd normally need to change after
wiring up this drivetrain is there:

- `PORT_LEFT_DRIVE`, `PORT_RIGHT_DRIVE` -- the Smart Ports the two MC55
  units are plugged into.
- `LEFT_MOTOR_REVERSED` / `RIGHT_MOTOR_REVERSED` -- flip whichever side
  drives backward when you push the stick forward.
- `MAX_DRIVE_VOLTAGE` -- safety cap and the "100% power" reference,
  defaults to 12V (standard VEX DC motor rating). Lower it for a gentler
  top speed.

This template assumes exactly one DC motor per side (two total). If your
drivetrain has more motors, you'll need additional MC55 units (one per
DC motor -- an MC55 does not control more than one motor), additional
`motor55` objects in `src/robot-config.cpp`, and a small change to
`drive-control.cpp`/`auton-helpers.cpp` to command all of them together
per side.

### Adding the optional sensors (for feedback-based autonomous)

Still in `config.h`, under "OPTIONAL SENSORS": flip `USE_INERTIAL_SENSOR`
and/or `USE_DRIVE_ROTATION_SENSORS` to `1`, set the port numbers for
whichever sensor(s) you wired up, and the matching helper functions
(`turnToHeading()` and/or `driveStraight()` in `auton-helpers.h/.cpp`)
compile in automatically -- no other file needs to change. Leave both at
`0` if you haven't added that hardware; the time-based autonomous
examples work completely on their own either way.

## Adding your own autonomous routine

Open `src/autons.cpp`:

1. Write a new function, e.g. `static void autonRightSide(void) { ... }`,
   using `driveForwardForTime(percent, seconds)` and
   `turnForTime(percent, seconds)` (or, if you've enabled the optional
   sensors, `driveStraight(inches)` / `turnToHeading(degrees)`).
2. Add it to the `autonList[]` array: `{"Right Side", autonRightSide}`.
3. That's it -- it now shows up in the on-screen selector in
   `autonSelectorRun()`, which runs automatically from `pre_auton()`
   before the match starts. Press the Brain-screen-cycling Up/Down
   buttons on the controller to choose it, A to confirm.

**Timing a move:** since there's no distance feedback, pick a power
(`AUTON_DRIVE_POWER_PERCENT` in `config.h` is the shared default),
download the program, run just that move on the actual field/floor you'll
compete on, and adjust the seconds until it travels/turns the amount you
want. Re-check it if the battery, floor, or robot weight changes
meaningfully -- open-loop timing is sensitive to all three.

## Servo practice (3-wire)

A `vex::servo` on a lettered 3-wire port is included for practice. It does **not** replace the MC55 drivetrain. Port letter, center (50°), jog step, and sweep speed are in `include/config.h`. What a servo is, how `setPosition` works, and the button procedure are in [docs/servo.md](docs/servo.md).

## Switching/choosing driver control mode

`config.h`'s `DEFAULT_DRIVE_MODE` picks TANK or ARCADE at boot. Drivers
can also flip modes live during a match by pressing **ButtonX** on the
controller (see `driveControlInit()` in `src/drive-control.cpp` if you'd
rather bind a different button).

## Tuning the optional PID gains

The six PID gains in `config.h` only matter if you've enabled
`USE_INERTIAL_SENSOR` and/or `USE_DRIVE_ROTATION_SENSORS` -- with both
left off, autonomous is purely time-based and there is nothing to tune
beyond "how many seconds."

If you do add those sensors, tune each PID loop (drive, then turn) in
this order:

1. **Set `kI = 0` and `kD = 0`.** Raise `kP` from a small value until the
   robot reliably reaches the target but visibly overshoots and
   oscillates once or twice before settling. That overshoot is expected
   and fine for now -- you want kP "strong enough to matter."
2. **Raise `kD`** from 0 until the oscillation from step 1 is mostly
   gone and the robot settles in one smooth motion instead of
   wobbling. Too much `kD` makes the motion feel sluggish/hesitant --
   back off if so.
3. **Only if** the robot still consistently stops a bit short (or past)
   the target every time, add a *small* `kI` (start at `0.0` and
   increase by tiny steps). Too much `kI` causes a slow wind-up wobble.
4. Re-check the matching `*_TOLERANCE_*` and `*_TIMEOUT_SEC` -- tolerance
   should be as tight as your drivetrain can reliably hit without timing
   out; the timeout should be generous enough that a normal move
   finishes on its own, not by hitting the timeout.
5. Tune drive and turn *separately*, one sensor/loop at a time.

Both optional moves run with hard power caps (`DRIVE_MAX_PERCENT` /
`TURN_MAX_PERCENT` in `config.h`) specifically so an over-tuned gain
can't send the robot violently out of control while you're testing.

## Verification -- please read before trusting this blindly

This repository was assembled **without access to the actual VEX
Robotics VS Code extension or its bundled V5 SDK/toolchain**: that
extension and its compiler (`clang` + `arm-none-eabi` + VEX's own V5
runtime libraries) are only distributed through the VS Code Marketplace
install, which requires a desktop VS Code session and downloads
machine-specific binaries -- neither is available in this cloud
environment (no GUI, no `code` CLI).

What **was** verified, by cross-checking the official VEX API docs
(`api.vex.com`) and VEX's own GitHub/knowledge base, plus several real,
published GitHub repositories containing an actual
`.vscode/vex_project_settings.json` generated by a current (2024-2026)
install of the extension:

- The project layout (`src/`, `include/`, `makefile`, `vex/mkenv.mk`,
  `vex/mkrules.mk`, `.vscode/vex_project_settings.json`,
  `.vscode/c_cpp_properties.json`, `.vscode/settings.json`) -- the
  `makefile`/`mkenv.mk`/`mkrules.mk` contents in this repo are copied
  verbatim from a real extension-generated project and were identical
  across every example checked, old and new. (This layout and build
  system are the same regardless of drive motor type.)
- The exact names and registration pattern for the three competition
  functions (`pre_auton`, `autonomous`, `usercontrol`,
  `Competition.autonomous(...)`, `Competition.drivercontrol(...)`) --
  unchanged across every example checked, from 2019-era VEXcode Pro V5
  through a 2026-dated real extension project.
- **The `vex::motor55` class specifically**, by fetching
  `api.vex.com/v5/home/cpp/Motors_and_MotorControllers/mc55.html`
  directly: all three constructor overloads, the fact that it has `spin`
  (configured-velocity or explicit voltage), `stop()` with no brake-mode
  argument, `setStopping` supporting only `coast`/`brake` (no `hold`,
  unlike Smart Motors), `setVelocity`, `setReversed`, and diagnostic
  getters (`current`, `voltage`, `temperature`, `getMotorType`,
  `installed`) -- and the explicit absence of any position/encoder
  method, confirming MC55-driven motors have no built-in feedback.
- `vex::rotation` (optional distance sensor) and `vex::inertial`
  (optional heading sensor) against their official `api.vex.com`
  reference pages, along with every other API call used in this
  template (`controller` axes/buttons, `competition::isEnabled`,
  `timer`, `Brain.Screen.*`).
- **`vex::servo`**, against
  `api.vex.com/v5/home/cpp/Motors_and_MotorControllers/servo_motor.html`
  (constructor `servo(triport::port&)`, `setPosition` percent 0–100 and
  degrees/turns, auto-center at 50 degrees) and the Blocks page for the
  same device (travel 0–100 degrees). See `docs/servo.md`.

What was **not** and **could not** be verified here:

- **The code was never compiled.** There is no VEX V5 toolchain in this
  environment (no `code` CLI, no bundled clang/arm-none-eabi, no V5
  runtime libraries -- those only arrive via the extension's own install
  step inside a real VS Code desktop session). `include/vex.h` in this
  repo is a plain-text stub, not the real header the extension
  generates. **Do not treat this as a successful build** -- it is a
  structurally and API-verified template, not a compiled-and-tested one.
- The exact current contents of `.vscode/c_cpp_properties.json` and
  `.vscode/settings.json` the extension writes for *your* installed SDK
  version -- these were taken from one real, published example and will
  likely be silently rewritten by the extension the first time you open
  this project, which is expected.
- Running on real hardware (actual DC motor response at a given voltage,
  real stopwatch timing for the example autons, PID response if you add
  the optional sensors) -- the time values and PID gains in this repo
  are reasonable starting points, not measured/tuned values for any
  specific robot.

**First thing to do after opening this in the real extension:** build it
once before changing anything, to confirm the toolchain and this
project's file layout agree with each other on your machine.
