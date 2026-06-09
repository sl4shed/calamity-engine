# Getting Started {#getting_started}

## 1. Install dependencies and required build tools:

```bash
# Arch Linux
sudo pacman -Syu git cmake make ninja sdl3 sdl3_image sdl3_ttf fmt spdlog cereal box2d

# Debian/Ubuntu 25 (older ubuntu does not have sdl3 packaged)
sudo apt install git cmake make ninja-build libsdl3-dev libsdl3-image-dev libsdl3-ttf-dev libfmt-dev libspdlog-dev libcereal-dev

# Fedora
sudo dnf install git cmake make ninja-build SDL3-devel SDL3_image-devel SDL3_ttf-devel fmt-devel spdlog-devel cereal-devel
```

## 2. Create a new repository using the [project template](https://github.com/sl4shed/calamity-engine-template)

## 3. Clone the repository **_recursively_**:

```
git clone https://github.com/YOUR_USERNAME_HERE/calamity-engine-template --recursive
cd calamity-engine-template
```

## 4. Build the project (pick the platform you want to compile to):

### Windows MSVC

<a id="windows"></a>

1. Download the latest version of [Visual Studio](https://visualstudio.microsoft.com/) and make sure you have `Desktop development with C++` toggled on.
2. Run these commands:

```
cd path/to/project/
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
```

3. Go to the build folder and open the main solution in Visual Studio
4. Right click the `box2d` project in the browser and go to `Properties`.
5. Change the C++ language version to C++20
6. Do the same for your main project
7. Either compile the solution from Visual Studio or run `cmake --build . --config Release` from the build folder
8. Copy the `calamity` folder from the root of the build folder to wherever your `.exe` is

Your game should run now! Please open an issue on GitHub if you find any problems in this build process that are related to Calamity Engine.

### Linux

<a id="linux"></a>

```bash
cd path/to/project
mkdir build
cd build

cmake -G Ninja ..
ninja
```

### Emscripten

<a id="emscripten"></a>
You will need the [Emscripten SDK](https://emscripten.org/) installed.

```bash
cd path/to/project
emsdk activate latest # do whatever the command tells you to do

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
```
