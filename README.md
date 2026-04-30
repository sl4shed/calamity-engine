<div style="width: 100%; display: flex; align-items: center; justify-content: center; flex-direction: column; gap: 10px;">
<img src="./assets/logo-full.png" style="width: 50%;"></img>
<h1>Calamity Engine</h1>
</div>


<div style="width: 100%; display: flex; align-items: center; justify-content: center; flex-direction: column; gap: 10px;">

![hackatime badge](https://hackatime-badge.hackclub.com/U0A9S13HQF3/calamity-engine?label=time+spent+working+on+ts&style=flat)
![documentation badge](https://img.shields.io/badge/documentation-available?style=flat&label=go+read+the&link=https%3A%2F%2Fcalamity.sl4shed.xyz)

</div>


A modular and cross-platform 2D game engine made in SDL3 and C++ built in mind with the Sony PSP.

# Index

 - [Getting Started](https://calamity.sl4shed.xyz/html/getting_started.html)
 - [Basic Notions](https://calamity.sl4shed.xyz/html/basic_notions.html)

# [Supported Platforms](https://calamity.sl4shed.xyz)

 - Linux
 - Windows
 - Sony PSP
 - Web browsers (emscripten)

# Libraries used

 - [SDL3](https://github.com/libsdl-org/SDL)
 - [SDL3_image](https://github.com/libsdl-org/SDL_image)
 - [SDL3_ttf](https://github.com/libsdl-org/SDL_ttf)
 - [SDL3_gfx](https://github.com/sabdul-khabir/SDL3_gfx)
 - [cereal](https://github.com/USCiLab/cereal)
 - [spdlog](https://github.com/gabime/spdlog)
 - [box2d](https://github.com/erincatto/box2d)
 - [SDL_GameControllerDB](https://github.com/mdqinc/SDL_GameControllerDB)
 - [doxygen-awesome-css](https://github.com/jothepro/doxygen-awesome-css)

# Optimization Sidequest
Due to my engine targeting older/underpowered platforms like the Sony PSP, I designed my engine's architecture with performance in mind from the start.

The physics and render loops are fully decoupled from one another. Physics runs at a fixed 60hz, which avoids both over-simulation and the "spiral of death" where a slow frame causes even slower subsequent frames.

Components and children are stored in `std::vector`s, so iterating over them in update/render/physicsUpdate is sequential memory access, which matters a lot on platforms with limited cache.

On the physics side, Box2D's substepping is exposed and set to 4 substeps by default. Also, the pixel-to-meter scaling that Box2D requires is baked into the shape at creation time (`scaledPolygon`, `scaledCircle`, `scaledCapsule`) rather than being multiplied in every physics update.

A lot of the end-user API is also function-based rather than property-based specifically to avoid having to diff previous values to detect changes in update loops which I did to optimize cycles for the more underpowered platforms like the PSP.

In the end, I had no issues regarding performance on the PSP! Memory usage also remains < 60mb on desktop platforms. The engine runs efficiently, whilst maintaining a user-friendly API.

# Aknowledgements

 - [Hack Club](https://hackclub.com) ❤️
 - My friends that are smarter than me ([@art0007i](https://github.com/art0007i/), [@misleadingname](https://codeberg.org/misleadingname), [@vianraaa](https://github.com/vianraaa), [@ttmso](https://codeberg.org/ttmso))