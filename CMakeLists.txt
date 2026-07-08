cmake_minimum_required(VERSION 3.11)

set(BUILD_TARGET "NATIVE" CACHE STRING "Build target: NATIVE | PSP | EMSCRIPTEN | 3DS")
set_property(CACHE BUILD_TARGET PROPERTY STRINGS NATIVE PSP EMSCRIPTEN 3DS)

if(MSVC)
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

if(BUILD_TARGET STREQUAL "3DS")
    set(CMAKE_TOOLCHAIN_FILE "$ENV{DEVKITPRO}/cmake/3DS.cmake"
        CACHE STRING "Toolchain file")
endif()

project(calamity)

if(MSVC)
    set(CMAKE_CXX_STANDARD 20 CACHE STRING "" FORCE)
    set(CMAKE_CXX_STANDARD_REQUIRED ON) # MSVC needs C++20 for box2d (for some reason)
else()
    set(CMAKE_CXX_STANDARD 17)
endif()


option(TRACY_ENABLE "" OFF)
option(CALAMITY_VENDORED "Use vendored dependencies" OFF)
option(CALAMITY_FIND_SDL3 OFF)
option(CALAMITY_FIND_SPDLOG OFF)

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
    backend/services/input/events.cpp
    backend/services/graphics/graphics.cpp
    backend/services/graphics/definitions.cpp
    backend/services/engine.cpp
    backend/services/physics/physics.cpp
    backend/services/physics/definitions.cpp
)

if(MSVC)
    # Hide console for release builds on Windows
    set_target_properties(${PROJECT_NAME} PROPERTIES 
        WIN32_EXECUTABLE $<CONFIG:Release>
    )
endif()

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
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CALAMITY_ROOT}/vendored/SDL_GameControllerDB/gamecontrollerdb.txt
            $<TARGET_FILE_DIR:${target}>/calamity/gamecontrollerdb.txt
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CALAMITY_ROOT}/assets/default.ttf
            $<TARGET_FILE_DIR:${target}>/calamity/default.ttf
        )
    endif()
endfunction()

if(BUILD_TARGET STREQUAL "EMSCRIPTEN" OR MSVC OR BUILD_TARGET STREQUAL "3DS")
    message("Set build type to Release since Emscripten and Windows require the release build type.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
    set(CALAMITY_VENDORED ON)
endif()

if(BUILD_TARGET STREQUAL "3DS")
    # include("$ENV{DEVKITPRO}/cmake/3DS.cmake")

    set(CMAKE_PREFIX_PATH "$ENV{DEVKITPRO}/portlibs/3ds" ${CMAKE_PREFIX_PATH})
    set(SDLTTF_PLUTOSVG OFF)
    set(CALAMITY_FIND_SPDLOG ON)
    set(CALAMITY_VENDORED ON)
    set(CALAMITY_FIND_BOX2D ON)
endif()

if(CALAMITY_VENDORED)
    target_compile_definitions(${PROJECT_NAME} PUBLIC CALAMITY_VENDORED)
    if(BUILD_TARGET STREQUAL "EMSCRIPTEN")
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
    endif()

    if(CALAMITY_FIND_SDL3)
        find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
    else()
        add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
    endif()

    add_subdirectory(vendored/SDL_image EXCLUDE_FROM_ALL)
    set(SDLTTF_VENDORED ON CACHE BOOL "" FORCE)
    add_subdirectory(vendored/SDL_ttf EXCLUDE_FROM_ALL)

    target_link_libraries(${PROJECT_NAME} PUBLIC SDL3::SDL3 SDL3_image::SDL3_image SDL3_ttf::SDL3_ttf)
    if(BUILD_TARGET STREQUAL "EMSCRIPTEN")
        target_compile_definitions(${PROJECT_NAME} PUBLIC __EMSCRIPTEN__)
        target_compile_options(${PROJECT_NAME} PUBLIC -sUSE_SDL=3 -sUSE_SDL_IMAGE=3)
        target_link_options(${PROJECT_NAME} PUBLIC -sUSE_SDL=3 -sUSE_SDL_IMAGE=3 -sUSE_SDL_TTF=3 -sUSE_FREETYPE=1 -sUSE_HARFBUZZ=1 -sALLOW_MEMORY_GROWTH=1)
    endif()

    if(MSVC)
        target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
    else()
        target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
    endif()


    if(CALAMITY_FIND_SPDLOG)
        find_package(fmt REQUIRED)
        target_link_libraries(${PROJECT_NAME} PUBLIC fmt::fmt)

        find_package(spdlog CONFIG REQUIRED)
        target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)
    else()
        add_subdirectory(vendored/fmt)
        target_link_libraries(${PROJECT_NAME} PUBLIC fmt::fmt)

        add_subdirectory(vendored/spdlog)
        target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)
    endif()

    set(CEREAL_INSTALL OFF CACHE BOOL "" FORCE)
    set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(SKIP_PORTABILITY_TEST ON CACHE BOOL "" FORCE)
    set(BUILD_SANDBOX OFF CACHE BOOL "" FORCE)
    target_include_directories(${PROJECT_NAME} PUBLIC vendored/cereal/include)

    if(CALAMITY_FIND_BOX2D)
        target_compile_definitions(${PROJECT_NAME} PUBLIC CALAMITY_FIND_BOX2D)
        find_package(box2d)
        target_link_libraries(${PROJECT_NAME} PUBLIC box2d)
    else()
        set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
        set(BOX2D_SAMPLES OFF CACHE BOOL "" FORCE)
        set(BOX2D_BENCHMARKS OFF CACHE BOOL "" FORCE)
        set(BOX2D_PROFILE OFF CACHE BOOL "" FORCE)
        set(BOX2D_VALIDATE OFF CACHE BOOL "" FORCE)
        set(BOX2D_UNIT_TESTS OFF CACHE BOOL "" FORCE)
        add_subdirectory(vendored/box2d EXCLUDE_FROM_ALL)
        target_link_libraries(${PROJECT_NAME} PUBLIC box2d)

        if(MSVC)
            set_target_properties(box2d PROPERTIES CXX_STANDARD 20)
            target_compile_features(box2d PUBLIC cxx_std_20)
        endif()
    endif()
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
endif()

if(TRACY_ENABLE)
    add_subdirectory(vendored/tracy)
    target_link_libraries(${PROJECT_NAME} PUBLIC Tracy::TracyClient)
endif()
