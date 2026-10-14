# VEX Setup — VS Code Extension, Toolchain, Build, Download, Debug

Step-by-step setup for the **official VEX Robotics VS Code extension** (not PROS). Assumes you already know C++ and use VS Code.

← [Back to learning path](vex-learning-path.md) · API reference: [vex-api-cheatsheet.md](vex-api-cheatsheet.md)

---

## What you're installing

| Component | Role |
|---|---|
| **VS Code** | Editor |
| **VEX Robotics extension** | Project creation, build/download, device info, firmware updates, IntelliSense wiring |
| **VEX Robotics Feedback extension** | Installed automatically alongside the VEX Robotics extension; sends feedback to VEX |
| **Microsoft C/C++ extension** | Required for IntelliSense/linting — the VEX extension prompts you to install it |
| **Microsoft Python extension** | Only needed if any project uses Python; skip for a pure-C++ team |

Official install reference: [Installing the VEX Extension, C/C++ Extension, and Python Extension in VS Code](https://kb.vex.com/hc/en-us/articles/21409418167444-Installing-the-VEX-Extension-C-C-Extension-and-Python-Extension-in-VS-Code).

**Note:** VEX has replaced the standalone **VEXcode Pro V5** app with this extension — VEXcode Pro V5 is end-of-life. If you find old tutorials referencing "VEXcode Pro V5," the extension workflow is similar but some UI (notably the graphical Devices wizard, see below) is not identical.

---

## Step 1 — Install VS Code

Download from [code.visualstudio.com](https://code.visualstudio.com/). Any OS VS Code supports (Windows/macOS; the extension also lists Linux community use, but VEX's own compatibility claim is Windows + macOS — test on your actual machines).

---

## Step 2 — Install the VEX Robotics extension

1. Open VS Code → **Extensions** (Ctrl/Cmd+Shift+X).
2. Search **VEX Robotics**.
3. Click **Install**. Wait — this also installs the **VEX Robotics Feedback** extension and adds a VEX icon to the Activity Bar.
4. Search **C/C++** → install Microsoft's C/C++ extension (prompted automatically if you skip this).
5. (Optional, Python only) Search **Python** → install Microsoft's Python extension.

All four should appear under **INSTALLED** in the Extensions sidebar.

---

## Step 3 — Create a new C++ project

1. Click the **VEX icon** in the Activity Bar.
2. Click **New Project**.
3. Choose platform: **V5**.
4. Choose language: **C++**.
5. Choose a template — search/browse for the **Competition Template**, or start from a **Blank/Empty Project** and add the competition hooks yourself (see [vex-learning-path.md § Competition template](vex-learning-path.md#the-competition-template-at-a-glance)). You can also **import** a project exported from VEXcode Pro V5.
6. Pick a folder and project name.

### What makes a project "valid" to the extension

The extension recognizes a project by the presence of **`.vscode/vex_project_settings.json`**. If you clone a project from Git and the build/download buttons don't appear, make sure you opened the folder that directly contains `.vscode/`, not a parent or sibling folder — this is the #1 reported "extension doesn't recognize my project" issue on the VEX Forum.

### Project layout (typical)

```
my-robot/
├── .vscode/
│   └── vex_project_settings.json   # marks this as a valid VEX project — commit this
├── include/
│   ├── vex.h                        # pulls in the whole VEX SDK + `using namespace vex;`
│   └── robot-config.h               # (convention) extern declarations for shared devices
└── src/
    ├── main.cpp                     # pre_auton / autonomous / usercontrol / main
    └── robot-config.cpp             # (convention) actual device constructors
```

`robot-config.h`/`robot-config.cpp` are a **convention**, not something the extension enforces — see the device-config note below.

---

## Step 4 — Device configuration (no graphical wizard yet)

This is the single biggest workflow difference from the standalone VEXcode V5 app and from drag-and-drop VEXcode: **the VS Code extension does not currently have a graphical "Devices" panel for C++.** VEX has stated a separate Robot Config extension is planned, but as of this writing device objects are **constructed by hand, directly in code**.

> **Hardware note:** this robot's drive motors are **DC motors through Motor Controller 55 (`vex::motor55`)**, not V5 Smart Motors — the example below uses `motor55`, which has a smaller constructor and no gear-cartridge concept. See [vex-api-cheatsheet.md § Primary Drive API: Motor Controller 55](vex-api-cheatsheet.md#primary-drive-api-motor-controller-55-vexmotor55) for the full API, and [project-context.md § Hardware](project-context.md#hardware-corrected-oct-8-2026-evening--authoritative) for why.

Pattern used by most teams (mirrors what the standalone app's wizard would have generated):

**`include/robot-config.h`**

```cpp
#ifndef ROBOT_CONFIG_H
#define ROBOT_CONFIG_H

#include "vex.h"

extern vex::brain Brain;
extern vex::motor55 LeftDrive;
extern vex::motor55 RightDrive;
extern vex::controller Controller1;

#endif
```

**`src/robot-config.cpp`**

```cpp
#include "robot-config.h"

vex::brain Brain;
vex::motor55 LeftDrive = vex::motor55(PORT1);
vex::motor55 RightDrive = vex::motor55(PORT2, true);  // reversed
vex::controller Controller1 = vex::controller();
```

**`src/main.cpp`** then just `#include "robot-config.h"` and uses `LeftDrive`, `RightDrive`, etc. — no device construction in `main.cpp` itself.

This split matters once you have more than a drivetrain: it keeps port numbers in **one file** that mechanical can review, instead of scattered through logic code. Exact Smart Port assignments for the two MC55 units, plus any sensors the team later adds, are still **open from the team** — see [six-week-plan.md § Blockers](six-week-plan.md#blockers--need-answers-from-maurice-with-deadlines).

If the team later upgrades to real V5 Smart Motors, the swap is small: change `motor55` → `motor`, add a `gearSetting` argument, and gain access to `spinFor`/`driveFor`/`turnToHeading` — see [vex-api-cheatsheet.md § Motor — optional, future-upgrade path](vex-api-cheatsheet.md#motor-vexmotor--optional-future-upgrade-path-v5-smart-motors).

---

## Step 5 — Connect the V5 Brain

1. Power on the Brain; connect via USB cable, or pair via the V5 Controller.
2. The extension's **VEX DEVICE INFO** panel shows the connected Brain's firmware version, linked controller status, and battery.
3. If VEXos is out of date, the extension can update it from this panel — do this before your first build/download if prompted.

---

## Step 6 — Build & Download

Official terminology: **Build & Download** (not "upload" — that's PROS wording).

1. Click the **Slot Selector** icon in the status bar/toolbar and choose a program slot (1–8) on the Brain.
2. Click **Build & Download**. The project compiles; if it succeeds, it downloads to the connected Brain in the selected slot.
3. Enable **Run After Download** (extension setting, or a toolbar toggle) if you want the program to start automatically after download instead of needing a separate **Play**.
4. Click **Play** to run the program in the selected slot without re-downloading.

Fix compile errors using the **Log Terminal** (see below) and the Problems panel.

---

## Step 7 — Run on the field

With a **V5 Competition Switch** or field control connected, `autonomous()` and `usercontrol()` follow real match timing. Without field control (practice at the shop), the exact behavior depends on your project — typically the program runs `pre_auton()` then sits until you trigger auton/driver control from the Brain or Controller competition menu. Don't assume shop-floor timing matches a real match; test with a competition switch before Oct 12 if your team has one.

---

## Debugging: `printf` and the Interactive Terminal

The extension provides **two terminals**, and this trips up almost every new team:

| Terminal | Shows |
|---|---|
| **Log Terminal** | Build output, build errors, download results |
| **Interactive Terminal** | `printf`/`std::cout` output from your running program, and lets you send raw keystrokes back |

`printf` output goes to the **Interactive Terminal**, not the Log Terminal. To use it:

1. Make sure the extension setting **"Enable User Terminal"** is on (VS Code Settings → search `vexcode.general.enableUserTerminal`).
2. Connect a Brain or Controller — the Interactive Terminal only appears once a device is detected. (Exception: a V5 Controller alone doesn't enumerate a user serial port.)
3. **Interactive Terminal output clears by default after every build+download** — don't rely on scrolling back past a redeploy.

### `printf` buffering gotcha (the #1 "nothing prints" bug)

```cpp
printf("Hello World");     // may print NOTHING — buffered, no flush
printf("Hello World\n");   // prints immediately — \n flushes the buffer
```

Always end debug prints with `\n`, or call `fflush(stdout);` if you need output without a newline.

```cpp
printf("Heading: %.1f\n", Inertial.heading(degrees));
```

---

## VEX Device Info panel features worth knowing

| Feature | Use |
|---|---|
| **Screenshot Brain screen** | Capture what's on the physical Brain display |
| **Brain Event Log** | Firmware-level event history for troubleshooting |
| **V5 Battery Medic** | Battery health diagnostics |
| **Set brain name / team number** | Writes team number to the Brain for competition |
| **Erase user program** | Clears a slot without downloading a replacement |
| **VEXos update** | Updates Brain or Controller firmware from the panel |

---

## Troubleshooting

| Problem | Likely cause | Fix |
|---|---|---|
| No build/download buttons visible | Wrong folder opened, or missing `.vscode/vex_project_settings.json` | **File → Open Folder** on the folder that directly contains `.vscode/` |
| `#include "vex.h"` shows red squiggles but project builds | A different C/C++ dev environment extension/setting is overriding IntelliSense | Command Palette → **C/C++: Clear Visual Studio Developer Environment**, then reload the window. Don't use "Apply Developer Environment" on a VEX project. |
| `printf` shows nothing | Missing `\n`, or Interactive Terminal not enabled/visible | Add `\n`; enable **"Enable User Terminal"**; confirm a Brain/Controller is connected |
| Download fails / times out | Cable, port conflict, or another program (VEXcode app) holding the serial port | Re-seat USB; close other VEX software; retry |
| Program downloads but doesn't start | **Run After Download** disabled | Enable the setting, or press **Play** manually after download |
| Wrong motor spins / spins the wrong way | Port mismatch, or `reverse` flag wrong in the constructor | Check `robot-config.cpp` against the physical port map |
| Settings edits don't take effect | Trailing comma or JSON syntax error in `settings.json` | Fix the JSON; VS Code's Problems view flags this |

---

## First-time user checklist

- [ ] VS Code + VEX Robotics extension + Microsoft C/C++ extension installed
- [ ] New V5 C++ project created from the Competition Template (or Blank + hooks added)
- [ ] `.vscode/vex_project_settings.json` present and committed
- [ ] Brain appears in VEX Device Info panel
- [ ] Project builds with **Build & Download** with zero errors
- [ ] One motor moves from the controller in `usercontrol()`
- [ ] `printf("hello\n")` visible in the **Interactive Terminal** (not the Log Terminal)
- [ ] `wait(20, msec)` at the bottom of the `usercontrol` while loop

---

← [Back to learning path](vex-learning-path.md) · Next: [vex-api-cheatsheet.md](vex-api-cheatsheet.md)
