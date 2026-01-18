cd build
psp-cmake  ..
make
read -p "run emulated? [y/n]: " answer
if [ "$answer" == "y" ]; then
    ppsspp /home/andrew/Projects/calamity-engine/build
fi
