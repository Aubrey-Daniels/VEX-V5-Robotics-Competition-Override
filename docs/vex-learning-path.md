# VEX Learning Path — VEX V5 Competition Programming (VS Code Extension + C++ SDK)

A beginner-to-competent guide for a college engineering society member who knows C++ but has zero VEX competition-programming experience, using the **official VEX Robotics VS Code extension and VEX C++ SDK** — **not PROS**.

> **Stack correction:** if you've seen PROS material referenced for this project (PROS CLI, `pros::Motor`, `initialize()`/`opcontrol()`), that was written before the stack was corrected on Oct 8, 2026. This doc and its companions are the current, accurate reference. See [project-context.md § Superseded materials](project-context.md#superseded-materials-do-not-use).
>
> **Hardware correction (same day, evening):** the drive motors are **DC motors through Motor Controller 55 (`vex::motor55`)**, not V5 Smart Motors — no built-in encoder, no gear cartridges. Everywhere below that mentions `vex::motor`, gear cartridges, or `spinFor`/`driveFor`/`turnToHeading` is marked as an **optional future-upgrade path**, not what this robot runs today. See [project-context.md § Hardware](project-context.md#hardware-corrected-oct-8-2026-evening--authoritative).

**Companion docs (read in any order after this overview):**

| Doc | What it covers |
|---|---|
| [vex-setup.md](vex-setup.md) | VS Code extension install, project creation, build/download, debugging |
| [vex-api-cheatsheet.md](vex-api-cheatsheet.md) | Motors, motor groups, sensors, controller, pneumatics — verified `vex::` signatures |
| [vex-autonomous-concepts.md](vex-autonomous-concepts.md) | Driver control, auton progression, tuning workflow, auton selector, when community libs matter |
| [six-week-plan.md](six-week-plan.md) | Week-by-week plan compressed for the Oct 12 checkpoint |
| [servo.md](servo.md) | 3-wire servo vs MC55: wiring, `setPosition`, center, button test |

---

## What is the VEX VS Code extension?

The **VEX Robotics extension** for VS Code is VEX's official, currently-supported way to write V5 C++ (and Python) code in a real IDE. It **replaces VEXcode Pro V5**, the previous standalone editor, which is now end-of-life. The extension:

- Creates new projects from templates (including the Competition Template)
- Builds and downloads your code to the V5 Brain
- Updates VEX firmware (VEXos) on the Brain and Controller
- Provides IntelliSense for the VEX C++ API (via Microsoft's C/C++ extension)
- Shows device info, battery health, and a hardware troubleshooting panel

It is **not** PROS. PROS is a third-party (Purdue SIGBots) open-source alternative with its own CLI, its own API (`pros::` namespace), and its own community library ecosystem (LemLib, EZ-Template, OkapiLib). This team uses VEX's own SDK — different namespace (`vex::`), different competition hook names, different build tooling. See [project-context.md](project-context.md) for the authoritative stack decision.

### Why this stack, for this team

| Reason | Detail |
|---|---|
| Official support | Maintained directly by VEX; firmware updates and API docs stay in sync |
| Real C++ toolchain in VS Code | Multi-file C++ projects, Git, familiar editor — same value proposition PROS offers, without leaving the vendor's own tooling |
| Matches current VEX documentation | Every `vex::` API page, tutorial, and the Override game manual assume this SDK |
| No PROS toolchain to install/maintain | One extension install, not a separate CLI + toolchain |

Trade-off: the extension's device-configuration workflow is less mature than PROS's or the old VEXcode app's — there's no graphical "Devices" wizard for C++ yet (VEX has said one is planned). You hand-write device constructors. See [vex-setup.md § Device configuration](vex-setup.md#step-4--device-configuration-no-graphical-wizard-yet).

---

## Current season context (game-agnostic skills)

The **2026–2027 VEX V5 Robotics Competition game is [Override](https://content.vexrobotics.com/docs/2026-2027/override/files/v5rc-override-1.0.pdf)**. Head-to-head matches have a **15-second autonomous period** and a **1:45 driver-control period** on a 12′×12′ field.

None of the programming skills in this guide are Override-specific. Tank/arcade drive, PID, and auton selectors transfer to every VRC season. Use Override only as motivation when practicing time budgets — 15 s auton is short; reliable routines beat ambitious ones.

---

## Project types

When you create a new project (VEX icon → **New Project** → platform **V5** → language **C++**), you choose a template:

| Template | What it gives you | When to use |
|---|---|---|
| **Competition Template** | `pre_auton`/`autonomous`/`usercontrol` hooks already wired to a `competition` object | **Default choice** — any project that will ever run on a competition field |
| **Blank/Empty Project** | Minimal `main.cpp`, no competition hooks | Rare — only if you're hand-building the hooks yourself for a reason |
| **Imported from VEXcode** | Converts a project exported from the standalone VEXcode app | Only if migrating an existing VEXcode (not PROS) project |

For this team: use the **Competition Template** for the real robot project. Everything below assumes that template.

---

## The competition template at a glance

Unlike PROS, VEX SDK projects **do have a `main()`** you can see and edit — but you still implement three hook functions that the SDK calls at the right time, plus a `competition` object that wires them up.

| Function | When it runs | Your job |
|---|---|---|
| `pre_auton()` | Once at program start; **before** auton or driver control | Device setup, IMU calibration, auton-selector UI loop |
| `autonomous()` | Start of the Autonomous period (~15 s in VRC) | Run the selected auton routine — no controller input |
| `usercontrol()` | Driver-control period (~1:45 in VRC) | Controller loop — **must not return** (use `while (true)`) |

```cpp
#include "vex.h"
using namespace vex;

competition Competition;

void pre_auton(void) {
  // calibration, sensor resets, auton-selector UI
}

void autonomous(void) {
  // run the selected auton routine
}

void usercontrol(void) {
  while (true) {
    // controller input → motor commands
    wait(20, msec);
  }
}

int main() {
  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  pre_auton();

  while (true) {
    wait(100, msec);
  }
}
```

**Field control flow:**

```
Power on → main() registers callbacks → pre_auton() runs
     → robot disabled until a match state starts
     → autonomous()   (auton period, Competition fires this)
     → usercontrol()  (driver period, loops until match end)
```

**Critical rules:**

1. Every polling loop (`usercontrol`, auton wait-loops) needs `wait(ms, msec)` — at least a few ms, typically 10–20 ms.
2. `usercontrol()` must **not return** — it's a `while (true)` loop by design.
3. Construct device objects (`motor`, `inertial`, `controller`, …) **once**, as globals — not inside `usercontrol()`'s loop.

Details and debugging: [vex-setup.md § Debugging](vex-setup.md#debugging-printf-and-the-interactive-terminal) · [Competition Template article (VEX Library)](https://kb.vex.com/hc/en-us/articles/360036286691-Using-the-Competition-Template-in-VEXcode-Pro-V5)

---

## Brain / controller workflow

| Step | What happens |
|---|---|
| **Write code** | In VS Code, with `#include "vex.h"` at the top of every file that uses VEX SDK calls |
| **Build & Download** | Compiles your project; if successful, sends it to the connected Brain in a chosen program **Slot (1–8)** |
| **Play** | Runs the downloaded program in that slot — or set **Run After Download** to skip this step |
| **Debug** | `printf`/`std::cout` output appears in the **Interactive Terminal** (not the Log Terminal) |
| **Field control** | A V5 Competition Switch or field control system drives real match timing (`autonomous()` → `usercontrol()`); without it, you trigger phases manually |

Full walkthrough: [vex-setup.md](vex-setup.md).

---

## Motors/sensors API (the `vex::` namespace)

Every class in the VEX SDK lives in the `vex` namespace: `vex::motor55` (this robot's drive motors), `vex::servo` (a 3-wire position servo — see [servo.md](servo.md)), `vex::motor` (V5 Smart Motors — optional future upgrade), `vex::inertial`, `vex::controller`, `vex::rotation`, `vex::distance`, `vex::optical`, `vex::digital_out`/`vex::pneumatics`. The template generates `using namespace vex;` at the top of your files, so in practice you write `motor55`, `inertial`, etc. without the prefix.

```cpp
motor55 LeftDrive = motor55(PORT1);
motor55 RightDrive = motor55(PORT2, true);  // reversed

LeftDrive.setVelocity(100, percent);   // every project starts at 50% by default
LeftDrive.spin(forward);
```

**This robot uses `motor55`, which has no built-in encoder** — no `position()`, no `spinFor`. Full verified signatures (and what that means for autonomous) are in [vex-api-cheatsheet.md § Primary Drive API: Motor Controller 55](vex-api-cheatsheet.md#primary-drive-api-motor-controller-55-vexmotor55) and [vex-autonomous-concepts.md](vex-autonomous-concepts.md#autonomous-progression-with-motor-controller-55-no-built-in-encoder).

---

## Driver control (preview)

You read joystick values with `Controller1.AxisN.position()` (range **−100 to 100**, percent) and send commands with `motor55`'s `setVelocity(velocity, percent)` + `spin(dir)` (there's no single-call `spin(dir, velocity, percent)` overload on `motor55`) or `spin(dir, voltage, volt)` directly, voltage capped at **±8V**.

| Style | Inputs | Best for |
|---|---|---|
| **Tank** | Left Y → left side, Right Y → right side | Skid-steer, intuitive for new drivers |
| **Arcade** | Left Y = throttle, Right X = turn | Fewer axes to coordinate; popular in VRC |

Always apply a **deadband** (ignore small values near zero) and optionally a drive curve (square or cubic scaling) so fine positioning is possible. Full treatment: [vex-autonomous-concepts.md § Driver control](vex-autonomous-concepts.md#driver-control).

---

## Autonomous progression (preview)

> **This robot's drive motors (`motor55`, via Motor Controller 55) have no built-in encoder** — no `spinFor`, `driveFor`, or `turnToHeading`. The ladder below is corrected for that: Stage 2 requires adding external hardware (a Rotation Sensor), not just a software call.

| Stage | Technique | Needs | When it's enough |
|---|---|---|---|
| 1 | `setVelocity()` + `spin()` + `wait()` (time-based) | Nothing extra | The practical default on this robot — "drive off the line and stop" |
| **upgrade →** 2 | Distance-based, using an **external Rotation Sensor** | A Rotation Sensor mounted on a wheel/axle (hardware addition) | Straight lines, consistent distances — only once that sensor exists |
| 3–4 | Hand-rolled PID on distance (Rotation Sensor) and/or heading (Inertial) | Rotation Sensor and/or Inertial Sensor — **no built-in `drivetrain`/`smartdrive` available with `motor55`** | Reliable stops, correction while moving |
| 5–6 | Odometry / community library | Out of scope this arc — and LemLib/EZ-Template are PROS-only regardless of motor type | Not planned for this team this season — see below |

See [vex-autonomous-concepts.md](vex-autonomous-concepts.md) for the full ladder, tuning workflow, auton selector pattern, and the **important correction** that LemLib/EZ-Template are PROS-only and don't run on this stack.

---

## Working with the mechanical subteam

Before writing subsystem code, get these from mechanical/design — put them in `robot-config.h`/`robot-config.cpp` (see [vex-setup.md § Device configuration](vex-setup.md#step-4--device-configuration-no-graphical-wizard-yet)) or a shared spreadsheet:

| Ask for | Why code needs it |
|---|---|
| **Port map** | Every MC55 unit, sensor, 3-wire port — mismatches are the #1 debug time sink |
| **Whether a Rotation Sensor will be added to the drivetrain** | Determines whether auton can move past time-based (see [vex-autonomous-concepts.md](vex-autonomous-concepts.md#autonomous-progression-with-motor-controller-55-no-built-in-encoder)) — `motor55` has no gearset/cartridge concept, so this replaces the old "motor gearset" question |
| **Wheel type + diameter** | Only needed once a Rotation Sensor exists — converts its encoder degrees → inches driven |
| **Drivetrain gear ratio** | Only needed once a Rotation Sensor exists — motor rotations → wheel rotations |
| **Subsystem list** | Intake, lift, climber — each gets its own class or namespace |
| **3-wire pneumatics wiring** | Which port, and whether "energized" means extend or retract (single- vs. double-solenoid) |
| **IMU mount orientation** (if/when added) | Affects which axis is "forward" for heading correction |
| **Auton start position(s)** | Which routines are legal from which tile |

**Suggested team rhythm:** mechanical produces a port map early → software stubs `robot-config.h` with placeholder values and comments → both update together when anything moves. Software attends at least one driver practice per week; mechanical attends auton testing when routines touch mechanisms. As of this writing (Oct 8), **the team is still brainstorming the scoring mechanism** (drivetrain motor type is now confirmed — see [project-context.md § Hardware](project-context.md#hardware-corrected-oct-8-2026-evening--authoritative)).

---

## Week path, compressed for the remaining calendar

**Today is ~Oct 8, 2026.** The original 6-week arc assumed starting Sep 21; because of the stack correction, there are realistically **~4 days left before the Oct 12 first-iteration checkpoint**, not 3 weeks. The full week-by-week breakdown — including the crash plan to Oct 12 and the Week 4–6 hardening plan through Nov 2 — lives in [six-week-plan.md](six-week-plan.md). Short version:

```
Oct 8–11 (crash sprint)   Install extension, create project, write/configure
                          device config, get teleop driving, get ONE auton
                          routine that reliably finishes
Oct 12                    First-iteration checkpoint (not a finish line)
Oct 13–18  (Week 4)       Harden: second auton, battery variance, subsystem
                          opcontrol wiring if hardware exists
Oct 19–25  (Week 5)       Subsystem auton integration, match simulation
Oct 26–Nov 1 (Week 6)     Second robot iteration, re-tune, wireless download,
                          code review, retro
```

Full detail, blockers, and done-when checklists: [six-week-plan.md](six-week-plan.md).

---

## Quick troubleshooting

| Symptom | Likely cause | Fix |
|---|---|---|
| No build/download buttons | Wrong folder opened (missing `.vscode/vex_project_settings.json`) | Open the folder that directly contains `.vscode/` |
| Robot disconnects/freezes in `usercontrol` | No `wait()` in the loop | Add `wait(20, msec)` at the end of every loop |
| Motor runs the wrong direction | `reverse` flag or port sign | Check the constructor against the physical wiring |
| `printf` shows nothing | Missing `\n`, or Interactive Terminal not connected | Add `\n`; enable "Enable User Terminal"; connect a device |
| Auton works once, drifts the next run | Open-loop (time-based) move — the default on this robot's `motor55` drivetrain | Add an external Rotation Sensor and switch to hand-rolled PID, or accept the variance and re-tune per session |
| `motor55.spin(forward, 50, percent)` won't compile | That overload doesn't exist on `motor55` | Use `setVelocity(50, percent)` then `spin(forward)` — see [vex-api-cheatsheet.md § Primary Drive API](vex-api-cheatsheet.md#primary-drive-api-motor-controller-55-vexmotor55) |
| "Motors feel slow" | Default velocity is 50%, or physically dividing voltage was tried | `setVelocity(100, percent)` in software — never a physical voltage divider, see [vex-autonomous-concepts.md § Myth check](vex-autonomous-concepts.md#myth-check-dont-physically-divide-battery-voltage) |
| LemLib/EZ-Template tutorial doesn't compile | Those libraries are PROS-only, and assume a Smart-Motor-style encoder this robot doesn't have anyway | Don't use them on this stack — see [vex-autonomous-concepts.md § Odometry and community libraries](vex-autonomous-concepts.md#odometry-and-community-libraries) |

More detail: [vex-setup.md § Troubleshooting](vex-setup.md#troubleshooting) · [vex-autonomous-concepts.md § Common mistakes](vex-autonomous-concepts.md#common-beginner-mistakes)

---

*Next step: [vex-setup.md](vex-setup.md) — get your machine and Brain talking.*
