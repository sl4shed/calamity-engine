cmake_minimum_required(VERSION 3.11)

project(calamity-engine)

add_executable(${PROJECT_NAME}
    main.cpp
    backend/node.cpp
    backend/graphics.cpp
    backend/engine.cpp
    backend/utils.cpp
    backend/components.cpp
    backend/file.cpp
    backend/audio.cpp
    backend/definitions.cpp
)

set(BUILD_TARGET "NATIVE" CACHE STRING "Build target: NATIVE or PSP")
set_property(CACHE BUILD_TARGET PROPERTY STRINGS NATIVE PSP)

if(BUILD_TARGET STREQUAL "PSP")
    message(STATUS "Building for PSP")
elseif(BUILD_TARGET STREQUAL "NATIVE")
    message(STATUS "Building for native")
else()
    message(FATAL_ERROR "Unknown BUILD_TARGET: ${BUILD_TARGET}")
endif()

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
find_package(SDL3_image REQUIRED CONFIG)

set(CMAKE_BUILD_TYPE Debug)

# assets directory
add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
)
add_dependencies(${PROJECT_NAME} copy_assets)


target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
)

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
