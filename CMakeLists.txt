cmake_minimum_required(VERSION 3.11)

project(calamity)

set(BUILD_TARGET "NATIVE" CACHE STRING "Build target: NATIVE | PSP | EMSCRIPTEN")
set_property(CACHE BUILD_TARGET PROPERTY STRINGS NATIVE PSP EMSCRIPTEN)

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
)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

if(BUILD_TARGET STREQUAL "PSP")
    message(STATUS "Building for PSP")
    find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-static)
    find_package(SDL3_image REQUIRED CONFIG)
    find_package(SDL3_ttf REQUIRED CONFIG)
    find_package(box2d)
    target_link_libraries(${PROJECT_NAME} PUBLIC box2d)

    target_link_libraries(${PROJECT_NAME} PUBLIC
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
    )
elseif(BUILD_TARGET STREQUAL "NATIVE")
    target_compile_definitions(${PROJECT_NAME} PUBLIC PLATFORM_NATIVE)
    message(STATUS "Building for native")
    find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
    find_package(SDL3_image REQUIRED CONFIG)
    find_package(SDL3_ttf REQUIRED CONFIG)

    # box2d
    find_package(box2d)
    target_link_libraries(${PROJECT_NAME} PUBLIC box2d)

    target_link_libraries(${PROJECT_NAME} PUBLIC
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
    )
elseif(BUILD_TARGET STREQUAL "EMSCRIPTEN")
    message(STATUS "Building for Emscripten")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
    add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
    add_subdirectory(vendored/SDL_image EXCLUDE_FROM_ALL)
    set(SDLTTF_VENDORED ON CACHE BOOL "" FORCE)
    add_subdirectory(vendored/SDL_ttf EXCLUDE_FROM_ALL)

    target_link_libraries(${PROJECT_NAME} PUBLIC SDL3::SDL3 SDL3_image::SDL3_image SDL3_ttf::SDL3_ttf)
    target_compile_definitions(${PROJECT_NAME} PUBLIC __EMSCRIPTEN__)
    target_compile_options(${PROJECT_NAME} PUBLIC -sUSE_SDL=3 -sUSE_SDL_IMAGE=3)
    target_link_options(${PROJECT_NAME} PUBLIC -sUSE_SDL=3 -sUSE_SDL_IMAGE=3 -sUSE_SDL_TTF=3 -sUSE_FREETYPE=1 -sUSE_HARFBUZZ=1 -sALLOW_MEMORY_GROWTH=1)
else()
    message(FATAL_ERROR "Unknown BUILD_TARGET: ${BUILD_TARGET}")
endif()

if(BUILD_TARGET STREQUAL "EMSCRIPTEN")
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

    set(CALAMITY_ROOT "${CMAKE_CURRENT_LIST_DIR}" CACHE INTERNAL "Path to engine root")
    message(STATUS "CALAMITY_ROOT = ${CALAMITY_ROOT}")
    function(calamity_apply_assets target)
        target_link_options(${target} PRIVATE
                --embed-file=${CALAMITY_ROOT}/vendored/SDL_GameControllerDB/gamecontrollerdb.txt@calamity/gamecontrollerdb.txt
                --embed-file=${CALAMITY_ROOT}/assets/default.ttf@calamity/default.ttf
        )
    endfunction()
else()
    find_package(fmt REQUIRED)
    target_link_libraries(${PROJECT_NAME} PUBLIC fmt::fmt)

    find_package(spdlog CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)

    find_package(cereal REQUIRED)
    target_link_libraries(${PROJECT_NAME} PUBLIC cereal::cereal)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/vendored/SDL_GameControllerDB/gamecontrollerdb.txt ${CMAKE_CURRENT_BINARY_DIR}/gamecontrollerdb.txt COPYONLY)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/assets/default.ttf ${CMAKE_CURRENT_BINARY_DIR}/default.ttf COPYONLY)
endif()
