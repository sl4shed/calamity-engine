# Creating a new project {#creating_a_new_project}
## Windows
planned :)

## Linux
1. Install dependencies and required build tools:
```bash
# Arch Linux
sudo pacman -Syu cmake make ninja sdl3 sdl3_image sdl3_ttf fmt spdlog cereal

# Debian/Ubuntu
sudo apt install cmake make ninja-build libsdl3-dev libsdl3-image-dev libsdl3-ttf-dev libfmt-dev libspdlog-dev libcereal-dev

# Fedora
sudo dnf install cmake make ninja-build SDL3-devel SDL3_image-devel SDL3_ttf-devel fmt-devel spdlog-devel cereal-devel
```
2. Create the project directory and create a `CMakeLists.txt` file:
```sh
mkdir your-project-name-here
cd your-project-name-here
touch CMakeLists.txt
```
3. Open `CMakeLists.txt` with your text editor of choice and modify the following example to your needs:
```cmake
cmake_minimum_required(VERSION 3.11)

project(your-project-name-here)

# Cross compilation :)
set(BUILD_TARGET "NATIVE" CACHE STRING "Build target: NATIVE | PSP | EMSCRIPTEN")
set_property(CACHE BUILD_TARGET PROPERTY STRINGS NATIVE PSP EMSCRIPTEN)

# Get calamity engine
include(FetchContent)
FetchContent_Declare(calamity
    GIT_REPOSITORY https://github.com/sl4shed/calamity-engine
    GIT_TAG main
)

# pass build target to engine
set(BUILD_TARGET ${BUILD_TARGET} CACHE STRING "" FORCE)
FetchContent_MakeAvailable(calamity)
target_link_libraries(${PROJECT_NAME} PRIVATE calamity)

# add all of your .cpp files here
add_executable(${PROJECT_NAME}
    main.cpp
)

# copy the assets directory
add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
)
add_dependencies(${PROJECT_NAME} copy_assets)

# extra stuff for different platforms
if(BUILD_TARGET STREQUAL "PSP")
    create_pbp_file(
        TARGET ${PROJECT_NAME}
        ICON_PATH NULL
        BACKGROUND_PATH NULL
        PREVIEW_PATH NULL
        TITLE ${PROJECT_NAME}
        VERSION 01.00
    )
endif()

if(BUILD_TARGET STREQUAL "EMSCRIPTEN")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME "index"
        SUFFIX ".html"
    )

    # embed assets into emscripten build
    target_link_options(${PROJECT_NAME} PRIVATE --embed-file ./assets/)
endif()
```
4. Look at the \ref getting_started "getting started guide" to create a hello world project.
5. If you want to compile for other platforms, check out the \ref cross_compilation "cross compilation guide!"