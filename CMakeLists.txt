cmake_minimum_required( VERSION 3.14 )

project(starjeweled VERSION 0.2.0)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(FATAL_ERROR "Only Linux system is supported.")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(sources
    src/main.cpp
    src/mouse.cpp
    src/overlay.cpp
    src/screencapture.cpp
    src/solver.cpp
    src/x.cpp)

set(BIN_TARGET "terminal")

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(X11 COMPONENTS Xfixes Xcomposite Xrandr Xrender Xtst REQUIRED)

add_executable(${BIN_TARGET} ${sources} )
set_target_properties(${BIN_TARGET} PROPERTIES AUTOMOC ON)
target_compile_options(${BIN_TARGET} PRIVATE -Wall -Wextra -pedantic)
target_include_directories(${BIN_TARGET} PRIVATE ${X11_INCLUDE_DIR} )
target_link_libraries(${BIN_TARGET}
                       Qt5::Core
                       Qt5::Widgets
                       X11::X11
                       X11::Xfixes
                       X11::Xcomposite
                       X11::Xrandr
                       X11::Xrender
                       X11::Xtst)
