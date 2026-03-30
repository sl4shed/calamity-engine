cmake_minimum_required(VERSION 3.11)

project(calamity-engine)

set(CMAKE_BUILD_TYPE Debug)
set(BUILD_TARGET "NATIVE" CACHE STRING "Build target: NATIVE | PSP | EMSCRIPTEN")
set_property(CACHE BUILD_TARGET PROPERTY STRINGS NATIVE PSP EMSCRIPTEN)

add_library(calamity STATIC
    # core
    backend/core/definitions.cpp
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
)

target_include_directories(calamity PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

if(BUILD_TARGET STREQUAL "PSP")
    message(STATUS "Building for PSP")
    find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-static)
    find_package(SDL3_image REQUIRED CONFIG)
    find_package(SDL3_ttf REQUIRED CONFIG)

    target_link_libraries(calamity PUBLIC
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
    )
elseif(BUILD_TARGET STREQUAL "NATIVE")
    target_compile_definitions(calamity PUBLIC PLATFORM_NATIVE)
    message(STATUS "Building for native")
    find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
    find_package(SDL3_image REQUIRED CONFIG)
    find_package(SDL3_ttf REQUIRED CONFIG)

    # box2d
    #find_package(box2d)
    #target_link_libraries(calamity PUBLIC box2d)

    target_link_libraries(calamity PUBLIC
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
    add_subdirectory(vendored/SDL_ttf EXCLUDE_FROM_ALL)

    target_link_libraries(calamity PUBLIC SDL3::SDL3 SDL3_image::SDL3_image)
    target_compile_definitions(calamity PUBLIC __EMSCRIPTEN__)
    target_compile_options(calamity PUBLIC -sUSE_SDL=3 -sUSE_SDL_IMAGE=3)
    target_link_options(calamity PUBLIC -sUSE_SDL=3 -sUSE_SDL_IMAGE=3 -sUSE_SDL_TTF=3 -sUSE_FREETYPE=1 -sALLOW_MEMORY_GROWTH=1)
else()
    message(FATAL_ERROR "Unknown BUILD_TARGET: ${BUILD_TARGET}")
endif()

if(BUILD_TARGET STREQUAL "EMSCRIPTEN")
    add_subdirectory(vendored/fmt)
    target_link_libraries(calamity PUBLIC fmt::fmt)

    add_subdirectory(vendored/spdlog)
    target_link_libraries(calamity PUBLIC spdlog::spdlog)

    target_compile_features(calamity PUBLIC cxx_std_17)
    set(CEREAL_INSTALL OFF CACHE BOOL "" FORCE)
    set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(SKIP_PORTABILITY_TEST ON CACHE BOOL "" FORCE)
    set(BUILD_SANDBOX OFF CACHE BOOL "" FORCE)
    target_include_directories(calamity PUBLIC vendored/cereal/include)
    #target_link_libraries(calamity PUBLIC cereal::cereal)
else()
    find_package(fmt REQUIRED)
    target_link_libraries(calamity PUBLIC fmt::fmt)

    find_package(spdlog CONFIG REQUIRED)
    target_link_libraries(calamity PUBLIC spdlog::spdlog)

    find_package(cereal REQUIRED)
    target_link_libraries(calamity PUBLIC cereal::cereal)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/vendored/SDL_GameControllerDB/gamecontrollerdb.txt ${CMAKE_CURRENT_BINARY_DIR}/gamecontrollerdb.txt COPYONLY)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/assets/default.ttf ${CMAKE_CURRENT_BINARY_DIR}/default.ttf COPYONLY) 
endif()
