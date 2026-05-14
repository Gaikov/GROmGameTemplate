# AGENTS.md

## Setup

- `GROmEngine/` is a **git submodule**. After cloning, run `git submodule update --init`.
- On macOS, GLFW is required: `brew install glfw`.

## Build commands

| Target | Command |
|--------|---------|
| Desktop (native) | `cmake -S ./ -B ./cmake-build-debug -DCMAKE_PREFIX_PATH=$(brew --prefix) && cmake --build ./cmake-build-debug` |
| WASM (full) | `./wasm-prepare.sh` (requires `~/emsdk`) then `./wasm-build.sh` |
| WASM (quick) | `./build.sh` (expects `emcc`/`em++` on PATH, simpler) |
| Android debug | `cd Android && ./gradlew assembleDebug` |

No lint, format, or C++ test commands exist.

## Runtime

- The **working directory must be `assets/`** when running the executable. All asset paths are relative.
- WASM output lands in `bin/`: `index.html`, `index.js`, `index.wasm`, `index.data`.

## Architecture

- **Entry points**: `GameSources/GameApp.cpp` (desktop/WASM), `Android/app/src/main/cpp/main.cpp` (Android — `android_main()`).
- **Factory functions**: The engine calls `App_GetGame()` → `IGameApp*` and `App_GetInfo()` → `IGameAppInfo*`. Do not change these signatures.
- Game logic lives in `GameSources/`. Android glue (renderer, input, JNI) lives in `Android/app/src/main/cpp/`.

## C++ standards

- Root CMake sets **C++20** for desktop/WASM.
- Android Gradle build (`build.gradle.kts`) overrides to **C++17** via `cppFlags "-std=c++17"`. Keep this mismatch in mind.

## WASM quirks

- Forces WebGL **1.0** only (`-sUSE_WEBGL2=0 -sMAX_WEBGL_VERSION=1 -sLEGACY_GL_EMULATION=1`).
- Uses `shell_minimal.html` from `GameSources/` as the Emscripten shell file.
- `build.sh` passes compilers directly (no toolchain file); `wasm-prepare.sh` uses the full Emscripten toolchain + Ninja.

## Android notes

- Assets are shared from root: `assets.srcDirs("../../assets")` in `build.gradle.kts`.
- Keystore credentials are hardcoded in `build.gradle.kts` — store/release passwords are `"pass"`, alias `"grom"`.
- Android NDK build links against `game-activity`, EGL, and OpenGL ES 1.0 (`glEsVersion=0x00010000`).
- Java tests: `./gradlew test` (unit) and `./gradlew connectedAndroidTest` (instrumented).

## Known issues (GameSources/todo.md)

- `IsMobile` detection for web is not implemented
- Sound activation for web is incomplete
- Pixel-to-pixel canvas sizing is broken
