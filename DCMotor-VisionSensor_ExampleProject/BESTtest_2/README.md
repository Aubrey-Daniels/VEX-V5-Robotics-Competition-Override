# BESTtest_2: VEX V5 Example (Motor + AI Vision)

A small, heavily commented example for the **VEX Robotics Extension for VS Code** (C++). It shows how to:

1. Declare hardware (Brain, motor, AI Vision sensor) in one place.
2. Use the standard VEX **competition template** (`pre_auton`, `autonomous`, `usercontrol`).
3. Spin a motor with voltage control.
4. Run a **background task** that reads the AI Vision sensor and prints results to the Brain screen.

## Folder layout

| File | Purpose |
|------|---------|
| `src/main.cpp` | Program entry point and competition functions |
| `src/robot-config.cpp` / `include/robot-config.h` | Hardware definitions (ports, color signatures) and their `extern` declarations |
| `src/motor-demo.cpp` / `include/motor-demo.h` | Motor example |
| `src/vision-demo.cpp` / `include/vision-demo.h` | AI Vision example |
| `include/vex.h` | Master include (VEX libraries + helper macros) |
| `makefile`, `vex/`, `.vscode/` | Created by the extension; leave as is |

Pattern used: each `.cpp` file has a matching `.h` in `include/` that declares what it offers to other files.

## Hardware used

| Device | Port | Where to change |
|--------|------|-----------------|
| 5.5W Motor (`motor1`) | 5 | `src/robot-config.cpp` |
| AI Vision sensor | 10 | `src/robot-config.cpp` |

## Build and run

1. Open the folder (or the `.code-workspace`) in VS Code with the VEX extension installed.
2. Connect the V5 Brain over USB.
3. Use the extension's **Build**, then **Download/Run** buttons.

On the Brain you should see "AI Vision Connected!" on row 1 and a red-object status on row 2, while the motor runs 10 seconds at a time.

## Ideas for extending it

- Add a controller and drive the motor from a joystick inside `usercontrol()`.
- Use the red object's position (`objects[0].centerX`) to steer toward it in `autonomous()`.
- Add more color signatures in `robot-config.cpp`.
