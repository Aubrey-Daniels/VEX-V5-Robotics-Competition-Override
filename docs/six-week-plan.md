# Six-Week VEX V5 + VEX VS Code Extension Plan — Maurice Colbert Jr. & Team

> **Stack correction (Oct 8, 2026):** this plan now targets the **official VEX Robotics VS Code extension + VEX C++ SDK**. The team does **not** use PROS. All PROS-specific plan content, timelines, and starter-code references below this line have been rewritten. **PROS materials elsewhere in `docs/` (`pros-learning-path.md`, `pros-setup.md`, `pros-api-cheatsheet.md`, `pros-autonomous-concepts.md`) are superseded — do not follow them.** See [project-context.md](project-context.md) for the authoritative decision record.
>
> **Hardware correction (Oct 8, 2026, evening):** the drivetrain is **2 DC motors through Motor Controller 55 (`vex::motor55`)**, not V5 Smart Motors. There is **no built-in encoder** — no gear cartridges, no `spinFor`/`driveFor`/`turnToHeading`. Steps below that referenced motor gearsets or distance-based moves have been corrected to time-based autonomous (the realistic default for this hardware) and `motor55` device config. See [project-context.md § Hardware](project-context.md#hardware-corrected-oct-8-2026-evening--authoritative) and [vex-autonomous-concepts.md § Autonomous progression with Motor Controller 55](vex-autonomous-concepts.md#autonomous-progression-with-motor-controller-55-no-built-in-encoder).

**Original window:** Mon Sep 21, 2026 → Mon Nov 2, 2026 (6 weeks), first-iteration checkpoint Mon Oct 12, 2026 (end of original "Week 3").
**Where we actually are:** today is **~Thu Oct 8, 2026**. The stack correction landed in what was originally Week 3, so **Weeks 1–3 of the old plan are effectively gone** — there was no VEX-SDK setup, driving, or auton work done against the correct stack yet. The realistic runway to the Oct 12 checkpoint is **~4 days**, not 3 weeks.
**This plan therefore has two parts:**
1. A **4-day crash sprint (Oct 8 → Oct 12)** replacing old Weeks 1–3, compressed and aggressive on purpose.
2. **Weeks 4–6 (Oct 13 → Nov 2)**, unchanged in spirit from the original plan but now scoped to VEX SDK work.

**Season game:** [Override 2026–2027](https://content.vexrobotics.com/docs/2026-2027/override/files/v5rc-override-1.0.pdf) — 15 s auton, 1:45 driver control, 12′×12′ field.

**Starting point:** a **VEX VS Code + `motor55` starter project already exists** on git branch `cursor/vex-vscode-starter-a093` — **use it, don't create a project from scratch.** Drivetrain motor type is confirmed (2 DC motors via Motor Controller 55, see [project-context.md § Hardware](project-context.md#hardware-corrected-oct-8-2026-evening--authoritative)); port map and subsystem list are still open with the team. Pull/checkout that branch and skip the "create project from the Competition Template" step below — keep the same device-config and auton goals, applied to the starter's existing files instead of a blank project.

**Companion docs (this store, VEX-SDK-accurate):**

- [vex-learning-path.md](vex-learning-path.md) — beginner hub, compressed week path, cross-links everything below
- [vex-setup.md](vex-setup.md) — extension install, project creation, build/download, device config, debugging
- [vex-api-cheatsheet.md](vex-api-cheatsheet.md) — Motor/MotorGroup/Controller/Inertial/Rotation/Distance/Optical/Pneumatics signatures
- [vex-autonomous-concepts.md](vex-autonomous-concepts.md) — auton ladder, PID tuning, selector pattern, and why LemLib/EZ-Template don't apply to this stack

---

## What "first iteration by Oct 12" means

Minimum viable robot + code — not a finished robot:

- [ ] Drivetrain physically built and driving — 2 drive motors via Motor Controller 55 (confirmed hardware)
- [ ] Teleop drive works from a controller (tank **or** arcade)
- [ ] Basic scoring mechanism functions manually, **if** the design has one by then (OK to slip this to Week 4–5 — see blockers below; it is explicitly not required for this checkpoint)
- [ ] Real Smart Port numbers for both MC55 units are entered in `robot-config.cpp` (not placeholders) — no gearset/wheel-ratio values needed yet, since `motor55` has no encoder to calibrate against
- [ ] **At least one auton routine that reliably finishes** — necessarily **time-based** (`spin`/`wait`/`stop`) unless a Rotation Sensor is added; doesn't need to score — "drive off the line and stop safely" counts
- [ ] Code is on the Brain via **Build & Download**, works from at least 2 people's machines, committed to Git

This is a **checkpoint, not a finish line** — Weeks 4–6 exist specifically to harden reliability, tune auton, and start a second iteration.

---

## Blockers — need answers from Maurice (with deadlines)

Because the stack correction compressed the timeline, **several of these are now needed immediately, not in Week 1–2 as originally planned.**

| Question | Why it blocks | Needed by | Fallback if unanswered |
|---|---|---|---|
| ~~Robot concept / drivetrain type~~ | **Resolved Oct 8 evening:** 2 drive motors via Motor Controller 55 (`vex::motor55`), ~12.8V battery | — | — |
| Exact Smart Port assignment for each of the 2 MC55 units | `robot-config.cpp` constructors need real `PORTx` values, not placeholders | **Oct 9 (tomorrow)** | Assume `PORT1`/`PORT2` as placeholders; correct once wired |
| Wheel diameter + drive gear ratio (if any Rotation Sensor is added later) | Needed only if/when a Rotation Sensor upgrade happens — not needed for the Oct 12 time-based checkpoint | **Oct 15** (Week 4, not blocking the crash sprint) | Not needed to hit Oct 12 — `motor55` has no encoder to calibrate against regardless |
| Will the team add a Rotation Sensor to the drivetrain this arc? | Determines whether auton stays time-based or moves to distance-based PID — see [vex-autonomous-concepts.md](vex-autonomous-concepts.md#autonomous-progression-with-motor-controller-55-no-built-in-encoder) | **Oct 15** (Week 4 decision, not Oct 12) | Assume **no** — plan Oct 12 around time-based auton only |
| Full port map (drive + any other sensor/subsystem that exists) | Every device needs a real, non-conflicting port before wiring | **Oct 11** (end of crash sprint) | Can't safely bring up anything beyond the drivetrain |
| Subsystem list (intake? lift? none yet?) | Determines what code gets written in Weeks 4–6 | **Oct 15** (early Week 4) | Plan stays drivetrain + auton only through Oct 12 |
| Practice field / tile access — how often? | Auton tuning needs the same surface as competition | **Oct 11** | Assume shop floor/carpet tuning first, re-tune on tile before Oct 12 if any access exists |
| Headcount: how many on code vs. building, in parallel? | Determines how much of the crash sprint can run side-by-side | **Oct 9** | Assume 1–2 people on code (Maurice + 1), rest on mechanical |
| Repo status (temp project → real repo) | Any PR for a starter project is blocked until a real repo exists | Before any PR is needed | Keep working on a branch; commit/push as usual until a repo exists |

---

## Assumptions (so work starts now, not after answers land)

- Drivetrain is **2 DC motors through 2× Motor Controller 55** (confirmed — not V5 Smart Motors, no gear cartridges, no built-in encoder).
- Team has (or will shortly have) a V5 Brain, a controller, and at least one drive motor + MC55 on hand for the crash sprint.
- No Rotation Sensor is assumed to exist yet — Oct 12's auton is planned as **time-based**, not distance-based, unless the team confirms otherwise before Day 4.
- Maurice is the primary programmer; 1–2 more people may co-drive code work once environment setup is done (see headcount blocker above).
- "Basic scoring mechanism" for Oct 12 is optional if the subsystem design isn't finalized — the milestone does **not** require it.
- The VEX VS Code + `motor55` starter on `cursor/vex-vscode-starter-a093` is the team's starting point for code — build on it, don't start a parallel blank project.

---

## Parallel tracks — what runs side by side

| Track | Oct 8–11 (crash sprint) | Oct 12 checkpoint | Weeks 4–6 |
|---|---|---|---|
| **Learning the VEX SDK** (Maurice, solo-friendly) | Install extension, pull the `cursor/vex-vscode-starter-a093` starter branch, read [vex-learning-path.md](vex-learning-path.md) + [vex-setup.md](vex-setup.md), get one motor spinning from a controller | Tune one auton routine, confirm teleop is clean | Add subsystem code, decide on a second auton, hardening |
| **Building hardware** (mechanical subteam) | Lock the drivetrain concept, start assembly — fully independent of code until wiring | Wire real ports, hand off the real port map | Subsystem build (intake/lift), second-iteration changes |
| **Where they must sync** | **End of Day 1 (today)** — need the robot concept decision to even start `robot-config.cpp` meaningfully | Daily — any port change must reach `robot-config.cpp` same day | Every driver-practice session |

Bottom line: **extension setup does not need to wait on the chassis.** Maurice can install the VEX extension, read `vex-setup.md`, and test teleop logic on a single motor while mechanical finalizes the drivetrain concept. They converge for real port wiring by Day 3–4 of the sprint.

---

## Crash sprint — Oct 8 to Oct 12 (replaces old Weeks 1–3)

This compresses "install + read + configure + drive + first auton" into four days. It is aggressive by design — the goal is a working checkpoint, not mastery.

### Day 1 — Thu Oct 8 (today): Environment + confirm wiring

**Learning goals:** [vex-setup.md](vex-setup.md) full install through Step 3 (project creation); [vex-learning-path.md § Competition template](vex-learning-path.md#the-competition-template-at-a-glance).

- [ ] Install VS Code + VEX Robotics extension + Microsoft C/C++ extension; confirm `.vscode/vex_project_settings.json` exists in a test project
- [ ] **Pull/checkout the existing starter branch `cursor/vex-vscode-starter-a093`** (VEX VS Code + `motor55` project) instead of creating a new project from the Competition Template
- [ ] **Mechanical: confirm which Smart Ports the 2 Motor Controller 55 units will use** — drivetrain motor type/count is already confirmed (2 DC motors via MC55), so this is a port-assignment task, not a design decision
- [ ] Read `vex-learning-path.md` top to bottom; skim `vex-api-cheatsheet.md § Primary Drive API: Motor Controller 55`

**Done when:** The `cursor/vex-vscode-starter-a093` starter builds as-is on at least one machine; Maurice can explain what `pre_auton()`, `autonomous()`, and `usercontrol()` each do without looking it up; mechanical has committed to Smart Port numbers for both MC55 units.

---

### Day 2 — Fri Oct 9: Device config + first motion

**Learning goals:** [vex-api-cheatsheet.md § Primary Drive API: Motor Controller 55](vex-api-cheatsheet.md#primary-drive-api-motor-controller-55-vexmotor55) and [§ Controller](vex-api-cheatsheet.md#controller-vexcontroller); [vex-setup.md § Device configuration](vex-setup.md#step-4--device-configuration-no-graphical-wizard-yet).

- [ ] Write `include/robot-config.h` / `src/robot-config.cpp` with `motor55` objects (not `motor`) on real or best-guess Smart Ports for the 2 drive motors (see Blockers table — use `PORT1`/`PORT2` placeholders if answers haven't landed)
- [ ] **Build & Download** to a connected Brain; confirm it runs
- [ ] Spin one motor from a controller button/axis in `usercontrol()` using `setVelocity()` + `spin(dir)` — confirm direction is correct and the motor isn't stuck at the default 50% velocity
- [ ] Set up `printf` debugging — confirm the Interactive Terminal shows output (see [vex-setup.md § Debugging](vex-setup.md#debugging-printf-and-the-interactive-terminal), including the `\n` gotcha)

**Done when:** Code is on the Brain, uploads from at least one more teammate's machine, and one motor responds correctly to a controller input.

---

### Day 3 — Sat/Sun Oct 10–11: Full teleop drive + IMU

**Learning goals:** [vex-autonomous-concepts.md § Driver control](vex-autonomous-concepts.md#driver-control); [vex-api-cheatsheet.md § Inertial Sensor](vex-api-cheatsheet.md#inertial-sensor-vexinertial) (only relevant if an Inertial Sensor is on hand).

- [ ] Wire real ports for both drive motors into `robot-config.cpp`; confirm/flip `reverse` flags so both sides drive the same way forward
- [ ] Implement tank **or** arcade drive (team's choice) with a 5–10% deadband, using the `setVelocity()` + `spin(dir)` pattern (not `spin(dir, velocity, percentUnits)` — that overload doesn't exist on `motor55`)
- [ ] If an Inertial Sensor is mounted: construct it, calibrate in `pre_auton()`, confirm `heading()` is stable (optional — not required for Oct 12)
- [ ] Confirm each motor reaches full speed at `setVelocity(100, percent)` — if it still feels weak, that's a motor power ceiling, **not** something to fix by altering voltage externally (see [vex-autonomous-concepts.md § Myth check](vex-autonomous-concepts.md#myth-check-dont-physically-divide-battery-voltage))

**Done when:** Robot drives straight, turns, and stops cleanly from a controller in the chosen drive mode; no placeholder ports remain for the drivetrain.

---

### Day 4 — Mon Oct 12: First auton + checkpoint

**Learning goals:** [vex-autonomous-concepts.md § Autonomous progression with Motor Controller 55](vex-autonomous-concepts.md#autonomous-progression-with-motor-controller-55-no-built-in-encoder) — `motor55` has no encoder, so this is a **time-based** (`spin`/`wait`/`stop`) routine, not `spinFor`/`driveFor` (which don't exist on this hardware).

- [ ] Write **one** auton routine: time-based straight move at minimum ("drive off the line and stop" is enough to pass the checkpoint) — tune the `wait()` duration empirically on the actual robot/battery
- [ ] Run it **10×**, log pass/fail — target ≥8/10 (expect more run-to-run variance than a sensor-based move would give; that's normal for time-based auton)
- [ ] If a scoring mechanism exists by now: add manual (button-triggered) control in `usercontrol()` — no auton integration required yet
- [ ] Commit to Git with a clear working-state message

**Done when — Oct 12 Milestone Checklist:**

- [ ] Teleop drive is smooth and directionally correct for both drivers
- [ ] At least one auton routine finishes within 15 s and succeeds **≥8/10 runs**
- [ ] `robot-config.cpp` has zero placeholder hardware values for anything actually built
- [ ] Code committed to Git with a working-state tag/commit message
- [ ] Team can demo: power on → auton runs → driver control takes over, without a laptop tether

---

## Week 4 — Oct 13–18: Harden + start subsystem code

**Learning goals:** [vex-autonomous-concepts.md § Auton selector pattern](vex-autonomous-concepts.md#auton-selector-pattern) and [§ Common beginner mistakes](vex-autonomous-concepts.md#common-beginner-mistakes).

- [ ] Add a second auton routine (different start position or a safer fallback) using an auton-selector pattern
- [ ] Test both autons on low **and** full battery
- [ ] If subsystem hardware exists: write its class/namespace (mirror the drivetrain file's structure), wire into `usercontrol()`
- [ ] Log every auton failure with `printf` — know *why*, not just *that*, it failed

**Mechanical goals:** build/mount the scoring subsystem if not already done; start iteration-2 structural changes (weight, durability, driver ergonomics).

**Programming goals:** second auton, subsystem opcontrol wiring, failure logging.

**Done when:** two selectable autons, both ≥8/10 reliable; any built subsystem is controllable in teleop.

---

## Week 5 — Oct 19–25: Subsystem auton integration + odometry go/no-go

**Learning goals:** [vex-autonomous-concepts.md § Odometry and community libraries](vex-autonomous-concepts.md#odometry-and-community-libraries) — read, then decide. **Note the correction:** LemLib and EZ-Template are PROS-only, don't run on this stack, and assume a Smart-Motor-style encoder this robot's `motor55` doesn't have anyway — "evaluate LemLib" from the old PROS plan does not apply here.

- [ ] Integrate subsystem actions into at least one auton (e.g., preload score)
- [ ] Decide, **with mechanical**: is time-based auton enough for this season, or is it worth adding a Rotation Sensor (and possibly an Inertial Sensor) to unlock hand-rolled PID/heading correction? This is now a hardware decision, not just a software one — see [vex-autonomous-concepts.md § Odometry and community libraries](vex-autonomous-concepts.md#odometry-and-community-libraries). (A full switch to PROS or to V5 Smart Motors to use LemLib/EZ-Template-style libraries is **out of scope** for this arc.)
- [ ] Run a full match simulation: auton → driver control back-to-back without a power cycle
- [ ] A second person on the team successfully builds/downloads code independently (spreads bus-factor risk)

**Mechanical goals:** second-iteration build changes based on Weeks 1–4 driving feedback (weight distribution, wiring management, durability fixes).

**Programming goals:** scoring auton, match simulation, odometry go/no-go decision.

**Done when:** one auton actually scores (not just drives); a second teammate can independently build+download; the match-sim run completes without a hidden `pre_auton()` bug.

---

## Week 6 — Oct 26–Nov 1 (wrap by **Mon Nov 2**): Second iteration + polish

**Learning goals:** [vex-setup.md § Troubleshooting](vex-setup.md#troubleshooting) and [vex-autonomous-concepts.md § Common beginner mistakes](vex-autonomous-concepts.md#common-beginner-mistakes) for event-day readiness.

- [ ] Second robot iteration assembled and driving (structural fixes from Weeks 1–5 feedback applied)
- [ ] Re-tune constants (velocity, PID if hand-rolled, deadband) if wheel size/weight/gear ratio changed in iteration 2
- [ ] Wireless download tested (confirm the extension can download over the Controller link, not just USB)
- [ ] Driver + code review against the [common mistakes table](vex-autonomous-concepts.md#common-beginner-mistakes) — zero blocking delays, all loops have `wait()`
- [ ] Team retro: what to prioritize next — odometry? more subsystems? drive practice cadence?

**Mechanical goals:** finish iteration 2; confirm nothing regressed from iteration 1's reliable auton.

**Programming goals:** re-tuning after mechanical changes, wireless download, code review, retro.

**Done when (Nov 2):** iteration 2 robot drives and runs its best auton at least as reliably as the Oct 12 milestone; wireless download confirmed; retro notes captured for the next arc.

---

## Open questions recap (send to Maurice)

1. ~~Robot concept / drivetrain type~~ — resolved: 2 DC motors via Motor Controller 55, ~12.8V battery
2. Exact Smart Port assignment for both MC55 units (needed by Oct 9)
3. Will the team add a Rotation Sensor (and/or Inertial Sensor) to the drivetrain this arc? Determines the auton ceiling for Weeks 4–6 (needed by Oct 15, not blocking Oct 12)
4. Full port map + subsystem list (needed by Oct 11 for drivetrain, Oct 15 for subsystems)
5. Practice field / tile access cadence
6. Headcount split: how many on code vs. build, and in parallel?
7. Timeline for converting the temp repo so a starter project's PR can land (if applicable)

---

*Source docs: [vex-learning-path.md](vex-learning-path.md) · [vex-setup.md](vex-setup.md) · [vex-api-cheatsheet.md](vex-api-cheatsheet.md) · [vex-autonomous-concepts.md](vex-autonomous-concepts.md) · [project-context.md](project-context.md)*

*Superseded (PROS, do not follow): `pros-learning-path.md` · `pros-setup.md` · `pros-api-cheatsheet.md` · `pros-autonomous-concepts.md`*
