# Calamity Engine
A modular, cross-platform engine made with C++ and SDL3.

# Build instructions 
```
git clone --recursive https://github.com/sl4shed/calamity-engine 
cd calamity-engine

# I haven't tried compiling this with windows so like good luck
# in general use the desktop cmake app

# install cereal, spdlog, sdl3, sdl3-image, sdl3-mixer and fmt
sudo pacman S spdlog sdl sdl3-image sdl3-mixer fmt

./run.sh # build for the platform you want
```