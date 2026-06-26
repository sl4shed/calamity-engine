cmake_minimum_required(VERSION 3.11)
project(calamity-workspace)

set(BUILD_TARGET "NATIVE" CACHE STRING "Build target: NATIVE | PSP | EMSCRIPTEN | 3DS")
set_property(CACHE BUILD_TARGET PROPERTY STRINGS NATIVE PSP EMSCRIPTEN 3DS)

# Compile the engine ONCE
add_subdirectory("../" "calamity")

# compile all examples
file(GLOB EXAMPLE_DIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*")

foreach(DIR ${EXAMPLE_DIRS})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${DIR} AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${DIR}/CMakeLists.txt)
        message(STATUS "Adding example project: ${DIR}")
        add_subdirectory(${DIR})
    endif()
endforeach()