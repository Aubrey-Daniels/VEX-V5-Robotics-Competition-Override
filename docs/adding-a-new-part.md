# Adding and Testing a New Part

How to try a new motor, sensor, or mechanism **without breaking `main` or anyone else's work**. This formalizes the pattern already used for the 3-wire servo (`include/servo-tests.h`, `src/servo-tests.cpp`, [servo.md](servo.md)) so every new part follows it instead of landing as a loose file on a branch.

← [Learning path](vex-learning-path.md) · [Project context](project-context.md)

**Why this exists:** before this pattern, example code for new parts (vision sensor, motor practice) was pasted directly onto branches with no `.vscode/` project files, no shared `config.h`, and sometimes the wrong SDK (`pros::` instead of `vex::`). It couldn't build, couldn't merge cleanly, and nobody else could pick it up. Every part now gets a small, consistent slot in the one real project.

---

## The pattern, in one picture

```
include/
  <part>-tests.h        <-- declares <part>TestsInit() / <part>TestsUpdate()
src/
  <part>-tests.cpp       <-- the actual test code for this one part
docs/
  <part>.md               <-- what the part is, wiring, and the button procedure
include/config.h          <-- add the part's port + any tuning numbers here, numbered section
src/main.cpp              <-- one line in pre_auton/usercontrol to call into it
```

This is exactly `servo-tests.h` / `servo-tests.cpp` / `docs/servo.md` — copy that trio as your starting template.

---

## Steps

- [ ] **Branch:** `git checkout -b <part>-tests` off current `main`
- [ ] **Wire the real port(s)** into a new numbered section of `include/config.h` (follow the existing numbering/comment style — one section per device, plain constants, no magic numbers elsewhere)
- [ ] **Build the device object** in `src/robot-config.cpp` (alongside `Brain`, `LeftDrive`, `RightDrive`, `TestServo`) — this is the **only** file that turns a config.h port into a real VEX SDK object
- [ ] **Write `include/<part>-tests.h` + `src/<part>-tests.cpp`:**
  - One `init()` function (reset state, print a startup line to the Brain screen)
  - One `update()` function, called every loop — read input, drive the part, print status
  - Pick controller buttons that aren't already used by drive or the auton selector (check `drive-control.cpp`, `autons.cpp`, and existing `*-tests.cpp` files first)
- [ ] **Wire it into `src/main.cpp`:** one `#include`, one `<part>TestsInit()` call near the other `*Init()` calls, one `<part>TestsUpdate()` call in the `usercontrol()` loop near `driveControlUpdate()`/`servoTestsUpdate()`
- [ ] **Write `docs/<part>.md`:** what the device is, which port it's on, and the exact button procedure to test it — model it on [servo.md](servo.md)'s structure (wiring → API → button test)
- [ ] **Build & Download, test on the actual robot** — confirm the drivetrain and any existing parts still work exactly as before (this is why each part gets its own file: a bug in `<part>-tests.cpp` shouldn't be able to break `drive-control.cpp`)
- [ ] **Open a PR back to `main`**, same habit as the rest of the repo — link the matching `docs/<part>.md` in the description so a reviewer knows what button does what

---

## Rules that keep this from turning back into a pile of loose files

1. **One part, one file pair, one doc.** Don't add a second device's code into an existing `*-tests.cpp` — copy the trio again with the new part's name.
2. **`vex::` only — never `pros::`.** This project does not use PROS (see [project-context.md § Stack](project-context.md#stack-corrected-oct-8-2026--authoritative)). If you're adapting an online PROS example, rewrite the API calls against [vex-api-cheatsheet.md](vex-api-cheatsheet.md) — don't paste `pros::` code and expect it to compile.
3. **Ports and tuning numbers only live in `config.h`.** If you catch yourself writing a bare port number or magic constant inside `<part>-tests.cpp`, move it to `config.h` instead.
4. **Never touch another part's file to test yours.** If testing your part means editing `drive-control.cpp` or `servo-tests.cpp`, something is wired wrong — ports and buttons should be independent.
5. **Decommission cleanly.** If a part gets abandoned (didn't work, design changed), delete its three files and remove its `config.h` section and `main.cpp` lines in one PR — don't leave dead code calling dead ports.
6. **Promote, don't duplicate, once it's real.** Once a test part is confirmed working and becomes an actual robot subsystem (e.g. an intake), fold its logic into a real subsystem file (mirroring `drive-control.h/.cpp`) instead of leaving it named `-tests` forever.

---

## Worked example: the servo

| Step | File |
|---|---|
| Port + tuning | `include/config.h` → `SERVO_3WIRE_PORT`, `SERVO_JOG_DEG`, `SERVO_SWEEP_DEG_PER_SEC` |
| Device object | `src/robot-config.cpp` → `TestServo = servo(Brain.ThreeWirePort.A)` |
| Test logic | `include/servo-tests.h` + `src/servo-tests.cpp` → `servoTestsInit()` / `servoTestsUpdate()` |
| Wired in | `src/main.cpp` → called from `usercontrol()`, not `pre_auton()` (so it doesn't fight the auton selector's buttons) |
| Doc | [servo.md](servo.md) → wiring, API, and the ButtonA/Up/Down/B procedure |

Use this as the literal template — rename `servo` to your part and follow the same five rows.

---

## If the part is a drivetrain upgrade, not a side practice device

A few parts aren't "extra" like the servo — they'd change how the robot actually drives (a Rotation Sensor or Inertial Sensor added to the drivetrain, for example). For those, don't gate them behind test buttons; follow the **opt-in flag** pattern already in `config.h` instead: `USE_INERTIAL_SENSOR` / `USE_DRIVE_ROTATION_SENSORS`, both default `0`. Flip to `1` once wired, and the matching helper in `auton-helpers.h/.cpp` compiles in with no other file changing. See [vex-autonomous-concepts.md § Autonomous progression with Motor Controller 55](vex-autonomous-concepts.md#autonomous-progression-with-motor-controller-55-no-built-in-encoder).

---

← [Learning path](vex-learning-path.md) · [Project context](project-context.md)
