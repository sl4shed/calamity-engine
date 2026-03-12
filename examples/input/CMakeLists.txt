cmake_minimum_required(VERSION 3.11)

project(calamity-inout-example)

add_executable(${PROJECT_NAME}
    main.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)


add_subdirectory("../../" "build")
target_link_libraries(${PROJECT_NAME} PRIVATE calamity)
set(CMAKE_BUILD_TYPE Debug)