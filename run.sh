read -p "build emscripten psp or native? [e/p/n]: " buildType

if [ "$buildType" == "p" ]; then
    cd build-psp
    psp-cmake -DBUILD_TARGET=PSP ..
    make -j6
    read -p "run? [y/n]: " answer
    if [ "$answer" == "y" ]; then
        PPSSPPSDL /home/andrew/Projects/calamity-engine/build
    fi
elif [ "$buildType" == "n" ]; then
    cd build
    cmake -DBUILD_TARGET=NATIVE ..
    make -j6
    read -p "run? [y/n]: " answer
    if [ "$answer" == "y" ]; then
        ./calamity-engine
    fi
elif [ "$buildType" == "e" ]; then
    emcmake cmake -S . -B build-emscripten -DBUILD_TARGET=EMSCRIPTEN ..
    cd build-emscripten
    emmake make -j6
    read -p "run? [y/n]: " answer
    if [ "$answer" == "y" ]; then
        emrun --no_browser --port 8080 .
    fi
else
    echo "unknown build type: $buildType"
fi
