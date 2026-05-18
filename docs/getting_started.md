# Getting Started {#getting_started}

## 1. Install dependencies and required build tools:
```bash
# Arch Linux
sudo pacman -Syu git cmake make ninja sdl3 sdl3_image sdl3_ttf fmt spdlog cereal box2d

# Debian/Ubuntu 25 (older ubuntu does not have sdl3 packaged)
sudo apt install git cmake make ninja-build libsdl3-dev libsdl3-image-dev libsdl3-ttf-dev libfmt-dev libspdlog-dev libcereal-dev

# Fedora
sudo dnf install git cmake make ninja-build SDL3-devel SDL3_image-devel SDL3_ttf-devel fmt-devel spdlog-devel cereal-devel

# Windows
# On windows, the only dependencies you need are the ones you need to build (visual studio C++ package (msvc), cmake, ninja).
```
## 2. Create a new repository using the [project template](https://github.com/sl4shed/calamity-engine-template)
## 3. Clone the repository ***recursively***:
```
git clone https://github.com/YOUR_USERNAME_HERE/calamity-engine-template --recursive
cd calamity-engine-template
```

## 4. Build the project (pick the platform you want to compile to):

### Windows MinGW64
<a id="windows"></a>
Right now, Windows builds are broken. Unfortunately, due to time constraints, I couldn't figure out a consistent way to get games building on Windows. I left some of the steps I took here. As an alternative, building using the WSL is recommended.

1. Download MinGW64 from [this link](https://github.com/brechtsanders/winlibs_mingw/releases/download/11.2.0-10.0.0-ucrt-r1/winlibs-x86_64-posix-seh-gcc-11.2.0-mingw-w64ucrt-10.0.0-r1.zip). Extract it anywhere, and add the `bin` folder to your environment variables. 
2. Install [CMake](https://cmake.org/download/).
3. Open CMD, and verify you have successfully installed MinGW64 by 
running `g++ --version` and verify CMake is also installed: `cmake --version`.

4. Run these commands: 
```
cd path/to/project/
mkdir build
cmake -S . -B build -DCALAMITY_VENDORED=ON -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```
Now you should have an executable in the build folder! Please open an issue on GitHub if you find any problems in this build process that are related to Calamity Engine.

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