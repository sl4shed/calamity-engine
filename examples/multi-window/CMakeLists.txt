cmake_minimum_required(VERSION 3.11)

project(multi-window)

set(BUILD_TARGET "NATIVE" CACHE STRING "Build target: NATIVE | PSP | 3DS")
set_property(CACHE BUILD_TARGET PROPERTY STRINGS NATIVE PSP 3DS)

add_executable(${PROJECT_NAME}
    main.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

add_subdirectory("../../" "calamity")
target_link_libraries(${PROJECT_NAME} PRIVATE calamity)

calamity_apply_assets(${PROJECT_NAME})
if(BUILD_TARGET STREQUAL "PSP")
    create_pbp_file(
        TARGET ${PROJECT_NAME}
        ICON_PATH NULL
        BACKGROUND_PATH NULL
        PREVIEW_PATH NULL
        TITLE ${PROJECT_NAME}
        VERSION 01.00
    )
elseif(BUILD_TARGET STREQUAL "3DS")
    ctr_generate_smdh(${PROJECT_NAME}.smdh
        NAME "Multi Window"
        DESCRIPTION "Multi Window Example"
        AUTHOR "sl4shed"
    )
    ctr_create_3dsx(${PROJECT_NAME}
        #ROMFS ${CMAKE_CURRENT_SOURCE_DIR}/romfs
        SMDH ${PROJECT_NAME}.smdh
    )
endif()