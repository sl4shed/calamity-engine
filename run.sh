read -p "psp build? [y/n]: " psp
if [ "$psp" == "y" ]; then
    cd build-psp
    psp-cmake -DBUILD_TARGET=PSP ..
    make
    read -p "run? [y/n]: " answer
    if [ "$answer" == "y" ]; then
        PPSSPPSDL /home/andrew/Projects/calamity-engine/build
    fi
else
    cd build
    cmake -DBUILD_TARGET=NATIVE ..
    make
    read -p "run? [y/n]: " answer
    if [ "$answer" == "y" ]; then
        ./calamity-engine
    fi
fi
