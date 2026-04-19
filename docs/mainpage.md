# Calamity Engine {#mainpage}

A modular and cross-platform 2D game engine made in SDL3 and C++ built in mind with the Sony PSP.

## Index

 - \subpage creating_a_new_project
 - \subpage your_first_project
 - \subpage compiling_a_project
 - \subpage example_game

## Platforms Supported

 - [Linux](compiling_a_project#linux)
 - [Windows](compiling_a_project#windows)
 - [Sony PSP](compiling_a_project#psp)
 - [Web browsers (emscripten)](compiling_a_project#web)

## Libraries used

 - [SDL3](https://github.com/libsdl-org/SDL)
 - [SDL3_image](https://github.com/libsdl-org/SDL_image)
 - [SDL3_ttf](https://github.com/libsdl-org/SDL_ttf)
 - [cereal](https://github.com/USCiLab/cereal)
 - [spdlog](https://github.com/gabime/spdlog)
 - [box2d](https://github.com/erincatto/box2d)
 - [SDL_GameControllerDB](https://github.com/mdqinc/SDL_GameControllerDB)
 - [doxygen-awesome-css](https://github.com/jothepro/doxygen-awesome-css)

# Optimization Sidequest
Due to my engine targeting older/underpowered platforms like the Sony PSP, I designed my engine's architecture with performance in mind from the start.

The physics and render loops are fully decoupled from one another. Physics runs at a fixed 60hz, which avoids both over-simulation and the "spiral of death" where a slow frame causes even slower subsequent frames.

Components and children are stored in `std::vector`s, so iterating over them in update/render/physicsUpdate is sequential memory access, which matters a lot on platforms with limited cache.

The active sprite on each node is cached (`currentSprite`) so the renderer doesn't have to search the full component vector every frame just to find the sprite.

On the physics side, Box2D's substepping is exposed and set to 4 substeps by default. Also, the pixel-to-meter scaling that Box2D requires is baked into the shape at creation time (`scaledPolygon`) rather than being multiplied in every physics update.

A lot of the end-user API is also function-based rather than property-based specifically to avoid having to diff previous values to detect changes in update loops which I did to optimize cycles for the more underpowered platforms like the PSP.


## Aknowledgements

 - [Hack Club](https://hackclub.com) ❤️
 - My friends that are smarter than me ([@art0007i](https://github.com/art0007i/), [@misleadingname](https://codeberg.org/misleadingname), [@vianraaa](https://github.com/vianraaa), [@ttmso](https://codeberg.org/ttmso))