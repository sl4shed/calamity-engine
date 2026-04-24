cmake_minimum_required(VERSION 3.11)

project(audio)

set(BUILD_TARGET "NATIVE" CACHE STRING "Build target: NATIVE | PSP | EMSCRIPTEN")
set_property(CACHE BUILD_TARGET PROPERTY STRINGS NATIVE PSP EMSCRIPTEN)

option(CALAMITY_VENDORED "Use vendored dependencies" OFF)

add_executable(${PROJECT_NAME}
        main.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

add_subdirectory("../../" "calamity")
target_link_libraries(${PROJECT_NAME} PRIVATE calamity)

add_custom_target(copy_assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
)
add_dependencies(${PROJECT_NAME} copy_assets)

calamity_apply_assets(${PROJECT_NAME})
if(BUILD_TARGET STREQUAL "EMSCRIPTEN")
    set_target_properties(${PROJECT_NAME} PROPERTIES
            OUTPUT_NAME "index"
            SUFFIX ".html"
    )

    target_link_options(${PROJECT_NAME} PRIVATE --embed-file ./assets/)
elseif(BUILD_TARGET STREQUAL "PSP")
    create_pbp_file(
            TARGET ${PROJECT_NAME}
            ICON_PATH NULL
            BACKGROUND_PATH NULL
            PREVIEW_PATH NULL
            TITLE ${PROJECT_NAME}
            VERSION 01.00
    )
endif()