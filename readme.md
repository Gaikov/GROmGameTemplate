# GROm Game Template

Cross-platform game template based on [GROmEngine](https://github.com/Gaikov/GROmEngine).

Supported targets:

- macOS using Metal
- Windows and Linux using OpenGL ES 3
- Android using OpenGL ES 3
- WebAssembly using WebGL 2

The project uses C++20 and CMake. `GROmEngine` is included as a Git submodule.

## Clone

Clone the repository together with the engine:

```bash
git clone --recurse-submodules git@github.com:Gaikov/GROmGameTemplate.git
cd GROmGameTemplate
```

For an existing clone, initialize or update the engine with:

```bash
git submodule update --init --recursive
```

## Runtime Working Directory

Native executables must be started with `assets/` as the working directory because runtime asset paths are relative:

```bash
cd assets
../cmake-build-debug/GameSources/GameTemplate
```

IDE run configurations must use the same `assets/` working directory.

## macOS

Requirements:

- Xcode Command Line Tools
- CMake and Ninja
- GLFW and GoogleTest

Install dependencies with Homebrew:

```bash
xcode-select --install
brew install cmake ninja glfw googletest
```

Configure and build:

```bash
cmake -S . -B cmake-build-debug \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH="$(brew --prefix)"
cmake --build cmake-build-debug --parallel
```

Run:

```bash
cd assets
../cmake-build-debug/GameSources/GameTemplate
```

## Windows (MSYS2)

Use the **MSYS2 UCRT64** shell. Do not configure the project from the plain `MSYS` shell because it produces an MSYS executable instead of a native Windows executable.

Install the native MinGW toolchain and dependencies:

```bash
pacman -Syu
pacman -S --needed \
  git \
  mingw-w64-ucrt-x86_64-toolchain \
  mingw-w64-ucrt-x86_64-cmake \
  mingw-w64-ucrt-x86_64-ninja \
  mingw-w64-ucrt-x86_64-glfw \
  mingw-w64-ucrt-x86_64-gtest
```

Restart the UCRT64 shell after a full MSYS2 update, then configure and build:

```bash
cmake -S . -B build-windows \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug
cmake --build build-windows --parallel
```

Run from the assets directory:

```bash
cd assets
../build-windows/GameSources/GameTemplate.exe
```

If the loader cannot find a generated DLL, add its build directory to `PATH` before launching:

```bash
export PATH="$PWD/../build-windows/GROmEngine/ThirdParty/openal-soft:$PATH"
```

## Linux

Ubuntu/Debian requirements:

```bash
sudo apt update
sudo apt install \
  build-essential \
  cmake \
  ninja-build \
  libglfw3-dev \
  libopenal-dev \
  libgl1-mesa-dev \
  libglu1-mesa-dev \
  libgtest-dev
```

Configure and build:

```bash
cmake -S . -B build-linux \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug
cmake --build build-linux --parallel
```

Run:

```bash
cd assets
../build-linux/GameSources/GameTemplate
```

## Android

Requirements:

- Android Studio or Android command-line tools
- JDK 17
- Android SDK 36
- Android NDK `29.0.14206865`

Android Studio can install the required SDK and NDK from **SDK Manager**. The Gradle project is located in `Android/`.

Build a debug APK:

```bash
cd Android
./gradlew assembleDebug
```

On Windows:

```powershell
cd Android
.\gradlew.bat assembleDebug
```

The APK is generated at:

```text
Android/app/build/outputs/apk/debug/app-debug.apk
```

Build a release Android App Bundle:

```bash
cd Android
./gradlew bundleRelease
```

Release signing can be configured in an untracked `Android/keystore.properties`:

```properties
storeFile=/absolute/path/to/release.keystore
storePassword=...
keyAlias=...
keyPassword=...
```

The same values can be supplied through `GROM_ANDROID_STORE_FILE`, `GROM_ANDROID_STORE_PASSWORD`, `GROM_ANDROID_KEY_ALIAS`, and `GROM_ANDROID_KEY_PASSWORD`.

## WebAssembly

The WASM build requires Emscripten SDK `6.0.4` installed at `~/emsdk`. A different location can be supplied through the `EMSDK` environment variable.

Install the SDK:

```bash
git clone https://github.com/emscripten-core/emsdk.git ~/emsdk
cd ~/emsdk
./emsdk install 6.0.4
./emsdk activate 6.0.4
```

The build scripts load `emsdk_env.sh` themselves, so modifying the global shell profile is unnecessary.

Clean configure and build:

```bash
./wasm-prepare.sh
```

Incremental build:

```bash
./wasm-build.sh
```

Generated files are placed in `bin/`:

```text
index.html
index.js
index.wasm
index.data
```

Serve them through HTTP:

```bash
python3 -m http.server 8000 --directory bin
```

Open `http://localhost:8000/index.html`. Opening `index.html` directly from the filesystem will not load the WASM assets correctly.

## Build Types

Replace `Debug` with `Release` for optimized native builds:

```bash
cmake -S . -B build-release -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build-release --parallel
```

The WASM scripts currently produce a Release build by default.

## Privacy Policy Publishing

The template keeps an editable policy in `Materials/Publish/PrivacyPolicy.html`. Replace every
placeholder and review the text against the game's actual data usage, permissions, third-party
services, audience, and applicable legal requirements.

The optional `.github/workflows/publish-privacy.yml` workflow copies the policy into another GitHub
repository whose `public/` directory is deployed by a static hosting provider. Configure
`Materials/Publish/privacy-policy.json` for each derived game:

- `appName`: exact public name of the game;
- `publisherRepository`: destination repository in `owner/repository` form;
- `publisherBranch`: destination branch;
- `targetPath`: unique path under the destination repository's `public/` directory;
- `enabled`: set to `true` after the policy and publishing settings are ready.

Add a repository secret named `PRIVACY_PUBLISH_TOKEN` containing a fine-grained token with write
access only to the selected destination repository. Repository secrets are not copied when a new
repository is created from this template. Never commit credentials or access tokens.

Deploy the destination repository's `public/` directory with any static hosting provider and submit
the resulting stable public HTTPS URL to the relevant app stores.
