# AutoVista Stage 1 Foundation

Minimal C++17 / OpenGL 3.3 Core renderer. It opens a GLFW window, loads GLAD,
compiles the basic GLSL shaders, and draws an indexed colored triangle with
VAO/VBO/EBO. This is the shared foundation for later team modules.

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

Press Esc to close the window.
