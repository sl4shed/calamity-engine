# Compiling a project {#compiling_a_project}

<a id="windows"></a>
## Windows
### Native

To compile natively for Windows, make sure you have [CMake installed](https://cmake.org/download/) and the [latest MSVC](https://visualstudio.microsoft.com/vs/features/cplusplus/) version installed aswell (you get it with Visual Studio).

```bash
mkdir build
cd build
cmake -DBUILD_TARGET=NATIVE ..

```

### PSP/Emscripten
If you're on Windows and you want to cross-compile your game for the PSP or for the web, I recommend [installing the WSL](https://learn.microsoft.com/en-us/windows/wsl/install) and then reading the Linux compilation instructions.
This is because [PSPDEV does not support Windows natively](pspdev.github.io/installation/windows.html), and you would need to use the WSL anyways.

<a id="linux"></a>
## Linux
To compile natively for linux, just pass the `-DBUILD_TARGET=NATIVE` option into cmake:
```bash
mkdir build
cd build
cmake -G Ninja -DBUILD_TARGET=NATIVE ..
ninja
./project-name-here
```
<a id="psp"></a>
## Sony PSP
To compile for the PSP, you have to have [the PSPDEV SDK installed](https://pspdev.github.io/installation.html).
```bash
mkdir build-psp
cd build-psp
psp-cmake -DBUILD_TARGET=PSP ..
make
PPSSPPSDL ABSOLUTE/BUILD/FOLDER/PATH/GOES/HERE
```
<a id="web"></a>
## Web Browser (emscripten)
To compile for web, you have to have [the Emscripten SDK installed properly](https://emscripten.org/docs/getting_started/downloads.html).
```bash
# Make sure to have the SDK activated!
emsdk activate latest

emcmake cmake -DBUILD_TARGET=EMSCRIPTEN ..
emmake make
emrun # to test out your project
```