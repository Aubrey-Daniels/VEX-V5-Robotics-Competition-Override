# Project Context — Durable Decisions

Single source of truth for decisions that shouldn't need re-litigating. If something here conflicts with another doc, **this file wins** — fix the other doc.

---

## Repository (confirmed Oct 14, 2026 — authoritative)

This project's shared home is **[github.com/Aubrey-Daniels/VEX-V5-Robotics-Competition-Override](https://github.com/Aubrey-Daniels/VEX-V5-Robotics-Competition-Override)**. Work in feature branches off `main` and open a PR to merge — the repo already has that habit (see merged/closed PR #1).

- `main` is the only branch that should reflect what the team actually runs. Keep it buildable.
- Older branches (`Template-1`, `visionSensorExampleCode`, `Aubrey-Daniels-Motor-control-practice-1`) predate the stack decision below — see [Superseded materials](#superseded-materials-do-not-use).
- New hardware experiments get their own branch + a short doc — see [adding-a-new-part.md](adding-a-new-part.md). This replaces the previous habit of pasting one-off example files straight onto a branch with no project structure.

## Stack (corrected Oct 8, 2026 — authoritative)

**We use the official VEX Robotics VS Code extension + VEX C++ SDK ("VEXcode" / `vex::` namespace).**

**We do NOT use PROS.** Any earlier PROS-based planning, starter code, or docs in this project are **wrong for this team's stack** and are superseded. See [Superseded materials](#superseded-materials-do-not-use) below.

| Component | What we use |
|---|---|
| Editor | VS Code |
| Extension | **VEX Robotics** extension (publisher: VEX Robotics; marketplace id `VEXRobotics.vexcode`) — replaces the now end-of-life standalone VEXcode Pro V5 app |
| Language | C++ (VEX C++ SDK, `vex::` namespace) |
| Companion extension | Microsoft C/C++ extension (required for IntelliSense; the VEX extension prompts to install it) |
| Toolchain | Bundled/managed by the VEX extension (no separate manual GCC install needed on the supported path) |
| Build/upload terminology | **Build & Download** (not PROS's "upload") — see [vex-setup.md](vex-setup.md) |
| Device config | Written by hand in code (constructors) — the VS Code extension does **not** yet have a graphical device wizard (VEX has stated one is planned but unreleased as of this writing); the standalone VEXcode V5 app's Devices wizard is a different product |

**Why this matters:** PROS and the VEX SDK are different C++ APIs with different namespaces, competition hook names, and member function names. Code, tutorials, and library ecosystems (LemLib, EZ-Template, OkapiLib) written for PROS **do not run** on the official VEX SDK — they require the PROS toolchain. See [vex-autonomous-concepts.md § Odometry and community libraries](vex-autonomous-concepts.md#odometry-and-community-libraries) for the full implication.

---

## Hardware (corrected Oct 8, 2026, evening — authoritative)

**Drive motors are NOT V5 Smart Motors.** The robot uses **DC motors connected through a VEX Motor Controller 55 (MC55)** — the `vex::motor55` class, not `vex::motor`. This is a real API/capability difference, not just a naming difference:

| | V5 Smart Motor (`vex::motor`) | **This robot: Motor Controller 55 (`vex::motor55`)** |
|---|---|---|
| Port | Smart Port | Smart Port (MC55 is a smart-port legacy device, not 3-wire) |
| Encoder / position feedback | Built in | **None** — no `position()`, no `spinFor`/`spinToPosition`, no `isDone` |
| Gear cartridges (red/green/blue) | Yes | N/A — MC55 drives a plain 2-wire DC motor, no cartridge concept |
| Speed control | `spin()`/`setVelocity()` with velocity or voltage | `spin()`/`setVelocity()` — same idea, voltage range is **−8V to 8V**, not battery voltage |
| Built-in closed-loop drivetrain (`driveFor`/`turnFor`/`turnToHeading`) | Yes, via `drivetrain`/`smartdrive` | **Not available** — those classes require `motor`/`motor_group` (smart motors) as input |

**Confirmed hardware:**
- **Drivetrain: 2 motors** (one per wheel/side) — DC motors through 2× Motor Controller 55 units.
- **Battery: ~12.8 V** (standard VEX V5 battery).
- Full API reference: [vex-api-cheatsheet.md § Primary Drive API: Motor Controller 55](vex-api-cheatsheet.md#primary-drive-api-motor-controller-55-vexmotor55). V5 Smart Motor content in that doc is kept as a clearly-marked **optional future-upgrade path**, not what this robot runs today.

**Corrected misconception — "dividing voltage":** the team previously considered physically dividing/reducing battery voltage to the drive motors to fix "motors felt slow." **Don't do this.** Speed is controlled entirely in software via `setVelocity(percent)` or `spin(dir, voltage, volt)` — the MC55 regulates output via PWM internally, and every project starts at a **default 50% velocity**, which alone would make motors feel sluggish. A physical voltage divider wastes power as heat, adds an unpredictable voltage drop under load, and is not how speed control works on this platform. If motors still feel underpowered after setting velocity to 100%, that's a motor/gearing power ceiling (2-wire DC motors are lower-torque than V5 Smart Motors), not a wiring fix.

**No built-in encoder — what this means for autonomous:** because `motor55` reports no position, the SDK's distance-based moves (`spinFor`, `driveFor`, `turnToHeading`) are **not available** for this drivetrain. Autonomous is **time-based** (`spin()` + `wait()` + `stop()`) by default. The team can add external **Rotation Sensors** (separate smart-port devices, mounted on an axle or idler wheel) to get real distance/position feedback for hand-rolled PID — but that is an optional hardware upgrade, not something `motor55` provides on its own. See [vex-autonomous-concepts.md](vex-autonomous-concepts.md) for the corrected ladder.

---

## Timeline

- **Today:** ~Oct 8, 2026.
- **First iteration checkpoint: Monday, Oct 12, 2026** — originally planned as "end of Week 3" of a 6-week arc that started Sep 21. Because the stack correction landed in Week 3, **the realistic runway to Oct 12 is ~4 days**, not 3 weeks. The crash plan in [six-week-plan.md](six-week-plan.md) reflects that compression — it is aggressive on purpose.
- **Season arc:** 6 weeks total, through **Monday, Nov 2, 2026**. Weeks 4–6 (Oct 13 – Nov 2) are for hardening, a second auton, and a second robot iteration — not a continuation of basic setup.

## Season

**2026–2027 game: [Override](https://content.vexrobotics.com/docs/2026-2027/override/files/v5rc-override-1.0.pdf)** — 15 s autonomous period, 1:45 driver-control period, 12′×12′ field.

## Team / robot status

- The team is **still brainstorming the robot** — scoring mechanism design and subsystem list are not locked in as of this doc. Drivetrain motor count/type **is** now confirmed (2 DC motors via MC55, see Hardware above).
- **Hardware details still open:** exact Smart Port assignments for the two MC55 units, wheel size, drivetrain gear ratio (if any external gearing beyond the motor), and the full subsystem list are all still needed from the team. The crash plan and setup docs use clearly-marked placeholder values until real numbers land — see [six-week-plan.md § Blockers](six-week-plan.md#blockers--need-answers-from-maurice-with-deadlines).
- Primary programmer: **Maurice Colbert Jr.** — knows C++, new to VEX competition programming and to the VEX SDK specifically.

## Documentation map (current, stack-correct)

| Doc | Purpose |
|---|---|
| [vex-learning-path.md](vex-learning-path.md) | Beginner hub — start here |
| [vex-setup.md](vex-setup.md) | Install extension, create project, build, download, debug |
| [vex-api-cheatsheet.md](vex-api-cheatsheet.md) | Verified `vex::` signatures: motor, motor_group, controller, inertial, rotation, distance, optical, pneumatics/ADI |
| [vex-autonomous-concepts.md](vex-autonomous-concepts.md) | Tank/arcade, deadband, time→encoder→PID→IMU→odom ladder in SDK terms |
| [six-week-plan.md](six-week-plan.md) | Week-by-week plan, compressed to the Oct 12 checkpoint |
| [servo.md](servo.md) | 3-wire `vex::servo` vs MC55 drive: wiring, `setPosition`, button test |
| [adding-a-new-part.md](adding-a-new-part.md) | How to add/test a new hardware part without breaking `main` |

## Superseded materials (do not use)

Before this stack correction, early planning for this team referenced **PROS** (the Purdue SIGBots toolchain) and **EZ-Template**/**LemLib** (PROS-only chassis/odometry libraries) — this is visible in this repository's own git history on branches like `Template-1`, `visionSensorExampleCode`, and `Aubrey-Daniels-Motor-control-practice-1`, which contain `pros::`-namespaced code (e.g. `pros::Motor`, `pros::Vision`) and an `EZ-Template-Example-Project.zip`. **Those branches predate this decision and do not match this team's stack.** Do not merge them into `main` as-is, and do not follow their README (the old root README's PROS/EZ-Template install links are superseded by this file and [vex-setup.md](vex-setup.md)).

If any idea from those branches is still worth having (the Vision Sensor example is the main candidate), rebuild it against `vex::vision` using the pattern in [adding-a-new-part.md](adding-a-new-part.md) — don't just rename `pros::Vision` calls, the constructors and signal API differ.

---

*Last updated: Oct 14, 2026 — GitHub migration + "adding a new part" workflow added. Previous update: Oct 8, 2026, PROS → VEX SDK stack correction.*
