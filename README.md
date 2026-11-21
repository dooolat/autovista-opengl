# AutoVista — Car Showroom Configurator

AutoVista is an interactive 3D car showroom built with C++17 and OpenGL 3.3 Core. View three procedural car models on a rotating turntable, orbit around them, and change paint, finish, lighting, and reflections.

## Features

- Three selectable models: sedan, Jeep, and racing car. The models are assembled from cube and cylinder meshes.
- Rotating display platform with a parent-child transform relationship between the platform and the car.
- Showroom scene with a tiled floor, two walls, and three columns.
- Per-fragment Blinn–Phong lighting with ambient, diffuse, and specular terms; three independently toggleable studio lights.
- Two forward-facing headlight spotlights, toggled together with the car lamps.
- Procedurally generated floor texture with UV mapping, repeat wrapping, and mipmaps.
- Cubemap environment reflections on painted body panels.
- Four paint presets and matte/glossy finish settings.
- Smooth and flat shading modes.
- Mouse and keyboard orbit camera, on-screen controls help, and an FPS counter in the window title.

## Controls

| Control | Action |
|---|---|
| Left mouse drag | Orbit camera |
| Arrow keys | Orbit camera |
| Mouse wheel or `Q` / `E` | Zoom |
| `,` / `.` | Cycle through sedan, Jeep, and racing car |
| `1` / `2` / `3` | Toggle studio lights |
| `4` | Toggle car lamps and headlight spotlights |
| `5` / `6` / `7` / `8` | Select red / blue / gold / green paint |
| `M` | Switch matte / glossy finish |
| `S` / `F` | Smooth / flat shading |
| `Space` | Pause / resume the turntable |
| `9` | Toggle cubemap reflections |
| `H` | Show / hide controls help |
| `Esc` | Close the application |

The window opens at 1280 × 720. The FPS counter is displayed in the window title.

## Requirements

- Windows 10 or 11
- Visual Studio 2022 with **Desktop development with C++**
- CMake 3.24 or newer
- OpenGL 3.3 capable graphics driver
- Internet access for the first CMake configure; GLFW 3.5.1 and GLM 1.0.3 are fetched by CMake

GLAD source and its license are included in `third_party/glad`.

## Build and run

Open PowerShell in the repository root and run:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
.\build\Release\AutoVista.exe
```

## Team contributions

| GitHub account | Main contributions |
|---|---|
| Dulat (`dooolat`) | Orbit camera and controls, configurator and paint controls, control-help overlay, per-vertex normals, and car selection. |
| Daulet (`MrLucky06`) | Procedural showroom and car geometry; sedan, Jeep, and racing car variants. |
| Dias (`Dias-devs`) | OpenGL rendering foundation, shader-program module, smooth/flat shading, and cubemap environment setup. |
| Diar (`diar06kz`) | Blinn–Phong studio lighting, headlight spotlights, and material-finish rendering. |
| Murat (`Murat513`) | FPS counter and target window size, cubemap reflection integration, and performance measurements. |

See [CREDITS.md](CREDITS.md) for third-party library and asset credits.
