cmake_minimum_required(VERSION 3.11)

if (MSVC)
    message("Building for Windows")

    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(CMAKE_C_STANDARD_LIBRARIES "" CACHE STRING "" FORCE)
    set(CMAKE_EXE_LINKER_FLAGS "" CACHE STRING "" FORCE)
    set(CMAKE_SHARED_LINKER_FLAGS "" CACHE STRING "" FORCE)
    set(CMAKE_MODULE_LINKER_FLAGS "" CACHE STRING "" FORCE)
    set(SDLIMAGE_VENDORED ON CACHE BOOL "" FORCE)
    set(SDLIMAGE_AVIF OFF CACHE BOOL "" FORCE)
    set(SDLIMAGE_WEBP OFF CACHE BOOL "" FORCE)
    set(SDLIMAGE_TIF OFF CACHE BOOL "" FORCE)
    set(SDLTTF_VENDORED ON CACHE BOOL "" FORCE)
    set(DAV1D_ASM OFF CACHE BOOL "" FORCE)
endif()

project(calamity)

set(CMAKE_CXX_STANDARD 17)
set(BUILD_TARGET "NATIVE" CACHE STRING "Build target: NATIVE | PSP | EMSCRIPTEN | WINDOWS")
set_property(CACHE BUILD_TARGET PROPERTY STRINGS NATIVE PSP EMSCRIPTEN WINDOWS)

option(CALAMITY_VENDORED "Use vendored dependencies" OFF)

add_library(${PROJECT_NAME} STATIC
        # core
        backend/core/definitions.cpp
        backend/core/signal.cpp
        backend/core/node/components.cpp
        backend/core/node/node.cpp
        backend/core/ui/definitions.cpp
        backend/core/ui/label.cpp

        # utils
        backend/utils/utils.cpp
        backend/utils/logger.cpp
        backend/utils/file.cpp

        # services
        backend/services/audio.cpp
        backend/services/services.cpp
        backend/services/input/input.cpp
        backend/services/graphics.cpp
        backend/services/engine.cpp
        backend/services/physics/physics.cpp
        backend/services/physics/definitions.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

set(CALAMITY_ROOT "${CMAKE_CURRENT_LIST_DIR}" CACHE INTERNAL "Path to engine root")
message(STATUS "CALAMITY_ROOT = ${CALAMITY_ROOT}")
function(calamity_apply_assets target)
    if(BUILD_TARGET STREQUAL "EMSCRIPTEN")
        target_link_options(${target} PRIVATE
                --embed-file=${CALAMITY_ROOT}/vendored/SDL_GameControllerDB/gamecontrollerdb.txt@calamity/gamecontrollerdb.txt
                --embed-file=${CALAMITY_ROOT}/assets/default.ttf@calamity/default.ttf
        )
    else()
        configure_file(${CALAMITY_ROOT}/vendored/SDL_GameControllerDB/gamecontrollerdb.txt ${CMAKE_CURRENT_BINARY_DIR}/calamity/gamecontrollerdb.txt COPYONLY)
        configure_file(${CALAMITY_ROOT}/assets/default.ttf ${CMAKE_CURRENT_BINARY_DIR}/calamity/default.ttf COPYONLY)
    endif()
endfunction()

if(BUILD_TARGET STREQUAL "EMSCRIPTEN" OR MSVC)
    message("Set build type to Release since Emscripten and Windows require the release build type.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
    set(CALAMITY_VENDORED ON)
endif()

if(CALAMITY_VENDORED)
    target_compile_definitions(${PROJECT_NAME} PUBLIC CALAMITY_VENDORED)
    if (BUILD_TARGET STREQUAL "EMSCRIPTEN")
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
    endif()

    add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
    add_subdirectory(vendored/SDL_image EXCLUDE_FROM_ALL)
    set(SDLTTF_VENDORED ON CACHE BOOL "" FORCE)
    add_subdirectory(vendored/SDL_ttf EXCLUDE_FROM_ALL)

    target_link_libraries(${PROJECT_NAME} PUBLIC SDL3::SDL3 SDL3_image::SDL3_image SDL3_ttf::SDL3_ttf)
    if (BUILD_TARGET STREQUAL "EMSCRIPTEN")
        target_compile_definitions(${PROJECT_NAME} PUBLIC __EMSCRIPTEN__)
        target_compile_options(${PROJECT_NAME} PUBLIC -sUSE_SDL=3 -sUSE_SDL_IMAGE=3)
        target_link_options(${PROJECT_NAME} PUBLIC -sUSE_SDL=3 -sUSE_SDL_IMAGE=3 -sUSE_SDL_TTF=3 -sUSE_FREETYPE=1 -sUSE_HARFBUZZ=1 -sALLOW_MEMORY_GROWTH=1)
    endif()

    target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

    add_subdirectory(vendored/fmt)
    target_link_libraries(${PROJECT_NAME} PUBLIC fmt::fmt)

    add_subdirectory(vendored/spdlog)
    target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)

    set(CEREAL_INSTALL OFF CACHE BOOL "" FORCE)
    set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(SKIP_PORTABILITY_TEST ON CACHE BOOL "" FORCE)
    set(BUILD_SANDBOX OFF CACHE BOOL "" FORCE)
    target_include_directories(${PROJECT_NAME} PUBLIC vendored/cereal/include)

    set(BOX2D_SAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
    set(BOX2D_SAMPLES OFF CACHE BOOL "" FORCE)
    set(BOX2D_BENCHMARKS OFF CACHE BOOL "" FORCE)
    set(BOX2D_PROFILE OFF CACHE BOOL "" FORCE)
    set(BOX2D_VALIDATE OFF CACHE BOOL "" FORCE)
    set(BOX2D_UNIT_TESTS OFF CACHE BOOL "" FORCE)
    add_subdirectory(vendored/box2d EXCLUDE_FROM_ALL)
    target_link_libraries(${PROJECT_NAME} PUBLIC box2d)

    add_subdirectory(vendored/SDL3_gfx)
    target_link_libraries(${PROJECT_NAME} PUBLIC SDL3_gfx_Static)
    target_include_directories(${PROJECT_NAME} PUBLIC vendored/SDL3_gfx)
else()
    if(BUILD_TARGET STREQUAL "NATIVE")
        find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
    else()
        find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-static)
    endif()

    find_package(SDL3_image REQUIRED CONFIG)
    find_package(SDL3_ttf REQUIRED CONFIG)
    find_package(box2d)
    target_link_libraries(${PROJECT_NAME} PUBLIC box2d)

    target_link_libraries(${PROJECT_NAME} PUBLIC
            SDL3::SDL3
            SDL3_image::SDL3_image
            SDL3_ttf::SDL3_ttf
    )

    find_package(fmt REQUIRED)
    target_link_libraries(${PROJECT_NAME} PUBLIC fmt::fmt)

    find_package(spdlog CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)

    find_package(cereal REQUIRED)
    target_link_libraries(${PROJECT_NAME} PUBLIC cereal::cereal)

    find_package(SDL3_gfx REQUIRED)
    if(BUILD_TARGET STREQUAL "PSP")
        find_package(SDL3_gfx REQUIRED)
        target_include_directories(${PROJECT_NAME} PUBLIC ${SDL3_gfx_INCLUDE_DIR})
        target_link_libraries(${PROJECT_NAME} PUBLIC ${SDL3_gfx_LIB_DIR}/libSDL3_gfx.a)
    else()
        target_link_libraries(${PROJECT_NAME} PUBLIC SDL3_gfx)
    endif()
endif()

# sdl gfx
# i need to include it vendored regardless lmao

