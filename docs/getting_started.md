# Getting Started {#getting_started}

## Preface

> A Calamity Engine game should not be your first C++ project. Please learn C++ programming, compiling, linking, and debugging before working with Calamity Engine. There are [many resources for this online](https://learncpp.com). You should also be comfortable with CMake.

With that being said, here is the process for creating a new project and compiling it:

## 1. Install dependencies and required build tools:

```bash
# Arch Linux
sudo pacman -Syu git cmake make ninja sdl3 sdl3_image sdl3_ttf fmt spdlog cereal box2d

# Debian/Ubuntu 25 (older ubuntu does not have sdl3 packaged)
sudo apt install git cmake make ninja-build libsdl3-dev libsdl3-image-dev libsdl3-ttf-dev libfmt-dev libspdlog-dev libcereal-dev

# Fedora
sudo dnf install git cmake make ninja-build SDL3-devel SDL3_image-devel SDL3_ttf-devel fmt-devel spdlog-devel cereal-devel
```

For windows, download the latest version of [Visual Studio](https://visualstudio.microsoft.com/) and make sure you have `Desktop development with C++` toggled on. You should also have [CMake](https://cmake.org) installed.

## 2. Create a new repository using the [project template](https://github.com/sl4shed/calamity-engine-template)

## 3. Clone the repository **_recursively_**:

```
git clone https://github.com/YOUR_USERNAME_HERE/calamity-engine-template --recursive
cd calamity-engine-template
```

## 4. Build the project (pick the platform you want to compile to):

### Windows MSVC

<a id="windows"></a>

```bash
cd path/to/project/
mkdir build
cd build

cmake ..
cmake --build .
```

You can find the executable of the game in ``build/Debug`` or ``build/Release`` depending on what build type you chose.

### Linux

<a id="linux"></a>

```bash
cd path/to/project
mkdir build
cd build

cmake -G Ninja ..
ninja
```

You can find the executable of the game in the ``build`` folder.

### Emscripten

<a id="emscripten"></a>
You will need the [Emscripten SDK](https://emscripten.org/) installed.

```bash
cd path/to/project
emsdk activate 5.0.2 # you MUST use 5.0.2 for now. 

mkdir build-web
cd build-web
# Building for Emscripten requires building vendored libraries.
emcmake cmake -DCALAMITY_VENDORED=ON -DBUILD_TARGET=EMSCRIPTEN -DCMAKE_BUILD_TYPE=Release ..
emmake make

# to run the app:
emrun index.html
```

### PSP

<a id="psp"></a>
You will need the [PSPDEV SDK](https://pspdev.github.io/installation.html) installed.

You need to install the necessary dependencies using psp-pacman:

```bash
# Install required dependencies
psp-pacman -Syu sdl3 sdl3-image sdl3-ttf spdlog fmt cereal box2d
```

Then:

```bash
cd path/to/project
mkdir build-psp
cd build-psp
psp-cmake -DBUILD_TARGET=PSP -G Ninja ..
ninja # Yes, PSPSDK can build using Ninja!

# To run the game, you need to have PPSSPP installed
PPSSPPSDL $(pwd)
```
