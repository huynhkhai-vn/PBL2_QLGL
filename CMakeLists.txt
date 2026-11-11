
cmake_minimum_required(VERSION 3.16)

project(PBL2_QLGL LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(PBL2_QLGL
    gui_main.cpp
)

target_include_directories(PBL2_QLGL PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(PBL2_QLGL PRIVATE
    Qt6::Widgets
)
