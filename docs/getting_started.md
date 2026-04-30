# Getting Started {#getting_started}

## 1. Install dependencies and required build tools:
```bash
# Arch Linux
sudo pacman -Syu git cmake make ninja sdl3 sdl3_image sdl3_ttf fmt spdlog cereal
yay -S sdl3_gfx-git # on arch, I created this aur package for sdl3_gfx


# On any other distro that does not have sdl3_gfx packaged, you will have to build vendored. Sorry :(
# Debian/Ubuntu 25 (older ubuntu does not have sdl3 packaged)
sudo apt install git cmake make ninja-build libsdl3-dev libsdl3-image-dev libsdl3-ttf-dev libfmt-dev libspdlog-dev libcereal-dev

# Fedora
sudo dnf install git cmake make ninja-build SDL3-devel SDL3_image-devel SDL3_ttf-devel fmt-devel spdlog-devel cereal-devel

# Windows
# On windows, the only dependencies you need are the ones to build (visual studio C++ package (msvc), cmake, ninja) but you also need SDL3. Follow the install guide for how to install SDL3 on SDL's official wiki:
# https://wiki.libsdl.org/SDL3/README-windows
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
2. Download this [SDL3 MinGW release](https://github.com/libsdl-org/SDL/releases/download/release-3.4.4/SDL3-devel-3.4.4-mingw.zip). Extract it anywhere and note down the path.
3. Install [CMake](https://cmake.org/download/).
4. Open CMD, and verify you have successfully installed MinGW64 by 
running `g++ --version` and verify CMake is also installed: `cmake --version`.

5. Run these commands: 
```
cd path/to/project/
mkdir build
cmake -S . -B build -DCALAMITY_VENDORED=ON -DCALAMITY_FIND_SDL3=ON -G "MinGW Makefiles" -DSDL3_DIR=path/to/SDL3-3.4.4/cmake -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```
Now you should have an executable in the build folder! Please open an issue on GitHub if you find any problems in this build process that are related to Calamity Engine.

### Linux
<a id="linux"></a>
If you're on Arch Linux, you don't have to build using vendored libraries because I packaged sdl3-gfx in [an AUR package](https://aur.archlinux.org/packages/sdl3_gfx-git).

Otherwise, you do have to build using `-DCALAMITY_VENDORED=ON`.
```bash
cd path/to/project
mkdir build
cd build

# If you're on arch and have sdl3_gfx-git installed:
cmake -G Ninja ..

# If you're on any other distro and don't have sdl3_gfx installed:
cmake -G Ninja -DCALAMITY_VENDORED=ON ..
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
psp-pacman -Syu sdl3 sdl3-image sdl3-ttf sdl3-gfx spdlog fmt cereal box2d
```

> If psp-pacman cannot find sdl3-gfx, that means [my pull request on psp-packages](https://github.com/pspdev/psp-packages/pull/295) has not been merged yet. In this case, you should clone [my fork of psp-packages](https://github.com/sl4shed/psp-packages):
```bash
git clone https://github.com/sl4shed/psp-packages
cd psp-packages/sdl3-gfx
psp-makepkg -si # Install the package
```

Then:
```bash
cd path/to/project
mkdir build-psp
cd build-psp
psp-cmake -DBUILD_TARGET=PSP -G Ninja ..
ninja # Yes, PSPSDK can build using Ninja!
```