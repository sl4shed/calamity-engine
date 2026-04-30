# Getting Started {#getting_started}

1. Install dependencies and required build tools:
```bash
# Arch Linux
sudo pacman -Syu git cmake make ninja sdl3 sdl3_image sdl3_ttf fmt spdlog cereal
yay -S sdl3_gfx-git # on arch, I created this aur package for sdl3_gfx


# On any other distro that does not have sdl3_gfx packaged, you will have to build vendored. Sorry :(
# Debian/Ubuntu 25 (older ubuntu does not have sdl3 packaged)
sudo apt install git cmake make ninja-build libsdl3-dev libsdl3-image-dev libsdl3-ttf-dev libfmt-dev libspdlog-dev libcereal-dev

# Fedora
sudo dnf install git cmake make ninja-build SDL3-devel SDL3_image-devel SDL3_ttf-devel fmt-devel spdlog-devel cereal-devel
```
2. Create a new repository using the [project template](https://github.com/sl4shed/calamity-engine-template)

3. Build the project:

### Windows Mingw64
<a id="windows"></a>
Keep in mind, Windows MSVC builds are still very much experimental. Maybe they will work, maybe they won't.
The only testing environment I had available was a Windows 10 LTSC virtual machine.
Regardless, if you are actively developing projects in C/C++ you should probably be using Linux or the WSL anyways :)

*By the way, [to compile anything for the PSP you need WSL](https://pspdev.github.io/installation/windows.html)*

For the purpose of seeing if the examples or the project template build, it's probably fine. You will need the latest MSVC which you can obtain by installing [Visual Studio](https://visualstudio.microsoft.com/) with the C++ desktop apps package or something like that. 
```bash
mkdir build-win
cmake -B build-win -DCALAMITY_VENDORED=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release # queue MSVC

cd build-win
# Here, copy the assets and calamity folder to the output folder (the one with the .exe)
explorer .
```
### Windows WSL
For this, I recommend setting up an [Arch Linux WSL](https://wiki.archlinux.org/title/Install_Arch_Linux_on_WSL) installation to also have access to the SDL3_gfx-git AUR package needed for building without vendored libraries.
After you have WSL set up, follow the regular Linux building guide. (Of course, this won't output Windows executables. WSL has excellent support for X11 and Wayland though. For actual Windows builds of your games, just run MSVC. You will also need the WSL for PSP builds!)

### Linux
<a id="linux"></a>
```bash
mkdir build
cd build
# If you're on arch and have sdl3_gfx-git installed:
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug ..
# If you're on any other distro and don't have sdl3_gfx installed:
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCALAMITY_VENDORED=ON ..
ninja
```

### Emscripten
<a id="emscripten"></a>
You will need the [Emscripten SDK](https://emscripten.org/) installed.

```bash
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

```bash
# Install required dependencies
psp-pacman -Syu sdl3 sdl3-image sdl3-ttf sdl3-gfx spdlog fmt cereal box2d

mkdir build-psp
cd build-psp
psp-cmake -DBUILD_TARGET=PSP -G Ninja ..
ninja
```