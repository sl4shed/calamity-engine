# Compiling a project {#compiling_a_project}

<a id="windows"></a>
## Windows
Planned :)
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
To compile for web, you have to have [the Emscripten SDK installed](https://emscripten.org/docs/getting_started/downloads.html).
```bash
# Make sure to have the SDK activated!
emsdk activate latest

emcmake cmake -DBUILD_TARGET=EMSCRIPTEN ..
emmake make
emrun # to test out your project
```