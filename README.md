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

Drag with the left mouse button or use the arrow keys to orbit; use the wheel
or Q/E to zoom. Press Shift+comma (`<`) or Shift+period (`>`) to switch car
models, 1-3 to toggle studio lights, 4 to toggle the car lamps, 5-8 to choose
paint, M to switch matte or glossy finish, and Space to pause or resume the
turntable. Press H to show or hide controls and Esc to close.

## AI use note

OpenAI Codex assisted with implementing the paint-finish isolation in the scene renderer and fragment shader.
