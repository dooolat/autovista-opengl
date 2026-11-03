# AutoVista Stage 1 — Orbit Camera and Scene Prototype

This C++17 / OpenGL 3.3 Core prototype renders a rotating black cylinder with
a red `+` marker and a blue cube attached to it. The cylinder and cube share
the same parent rotation. The orbit camera uses a Y-up coordinate system and
supports mouse orbit and wheel zoom.

## Requirements

- Windows 10/11
- Visual Studio 2022 with Desktop development with C++
- CMake 3.24 or newer
- OpenGL 3.3 capable graphics driver
- Internet on first configure for GLFW 3.5.1 and GLM 1.0.3

GLAD source and license are included under `third_party/glad`.

## Build and run

From the project root in PowerShell:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
.\build\Release\AutoVista.exe
```

Hold the left mouse button and drag to orbit. Dragging right moves the cube
right on screen; dragging up moves it up. Use the mouse wheel to zoom. Press
Esc to close the window.
