#!/bin/bash

cd_build_dir() {
    if [ ! -d "$1" ]; then
        mkdir "$1"
    fi
    cd "$1"
}



read -p "build emscripten psp or native? [e/p/n]: " buildType

if [ "$buildType" == "p" ]; then
    cd_build_dir build-psp
    psp-cmake -DBUILD_TARGET=PSP ..
    cmake --build . -j$(nproc)
    read -p "run? [y/n]: " answer
    if [ "$answer" != "n" ]; then
        PPSSPPSDL /home/andrew/Projects/calamity-engine/build
    fi
elif [ "$buildType" == "n" ]; then
    cd_build_dir build
    cmake -DBUILD_TARGET=NATIVE ..
    cmake --build . -j$(nproc)

    read -p "run? [y/n]: " answer
    if [ "$answer" != "n" ]; then
        ./calamity-engine
    fi
elif [ "$buildType" == "e" ]; then
    cd_build_dir build-emscripten
    emcmake cmake -DBUILD_TARGET=EMSCRIPTEN ..
    emmake make -j$(nproc)
    read -p "run? [y/n]: " answer
    if [ "$answer" != "n" ]; then
        emrun --no_browser --port 8080 .
    fi
else
    echo "please pick an option. $buildType isnt on the list."
    echo "building native..."

        cd_build_dir build
    cmake -DBUILD_TARGET=NATIVE ..
    cmake --build . -j$(nproc)

    read -p "run? [y/n]: " answer
    if [ "$answer" != "n" ]; then
        ./calamity-engine
    fi
fi
