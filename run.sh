cd build-psp
psp-cmake  ..
make
read -p "run emulated? [y/n]: " answer
if [ "$answer" == "y" ]; then
    PPSSPPSDL /home/andrew/Projects/calamity-engine/build
fi
