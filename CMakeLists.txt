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
    backend/logger.cpp
)

set(BUILD_TARGET "NATIVE" CACHE STRING "Build target: NATIVE | PSP | EMSCRIPTEN")
set_property(CACHE BUILD_TARGET PROPERTY STRINGS NATIVE PSP EMSCRIPTEN)

if(BUILD_TARGET STREQUAL "PSP")
    message(STATUS "Building for PSP")
    find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-static)
    find_package(SDL3_image REQUIRED CONFIG)

    target_link_libraries(${PROJECT_NAME} PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
    )
elseif(BUILD_TARGET STREQUAL "NATIVE")
    target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_NATIVE)
    message(STATUS "Building for native")
    find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
    find_package(SDL3_image REQUIRED CONFIG)

    target_link_libraries(${PROJECT_NAME} PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
    )
elseif(BUILD_TARGET STREQUAL "EMSCRIPTEN")
    message(STATUS "Building for Emscripten")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
    add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
    add_subdirectory(vendored/SDL_image EXCLUDE_FROM_ALL)
    set(CMAKE_EXECUTABLE_SUFFIX ".html" CACHE INTERNAL "")

    target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)
    target_compile_definitions(${PROJECT_NAME} PRIVATE __EMSCRIPTEN__)
    target_compile_options(${PROJECT_NAME} PRIVATE -sUSE_SDL=3 -sUSE_SDL_IMAGE=3)
    target_link_options(${PROJECT_NAME} PRIVATE -sUSE_SDL=3 -sUSE_SDL_IMAGE=3 -sALLOW_MEMORY_GROWTH=1 --embed-file ./assets/)
else()
    message(FATAL_ERROR "Unknown BUILD_TARGET: ${BUILD_TARGET}")
endif()

set(CMAKE_BUILD_TYPE Debug)

# assets directory
add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
)
add_dependencies(${PROJECT_NAME} copy_assets)

# json.hpp
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(vendored/json EXCLUDE_FROM_ALL)
#add_library(nlohmann_json::nlohmann_json ALIAS nlohmann_json::nlohmann_json)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

if (BUILD_TARGET STREQUAL "NATIVE")
    #fmt
    add_subdirectory(vendored/fmt)
    target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)

    # spdlog
    find_package(spdlog REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)
    #target_link_libraries(${PROJECT_NAME} PUBLIC ${STAGING_DIR}/include/) # spdlog
endif()

# output stuff
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
endif()
