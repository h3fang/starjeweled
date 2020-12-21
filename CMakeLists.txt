cmake_minimum_required( VERSION 3.9 )

project(starjewled VERSION 0.2.0)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(sources
    src/x.cpp
    src/overlay.cpp
    src/main.cpp)

set(BIN_TARGET "openoffice")

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(X11 REQUIRED)

add_executable(${BIN_TARGET} ${sources} )
set_property(TARGET ${BIN_TARGET} PROPERTY AUTOMOC ON)

target_include_directories(${BIN_TARGET} PRIVATE
                    ${XRANDR_INCLUDE_DIR}
                    ${X11_INCLUDE_DIR} )

target_link_libraries( ${BIN_TARGET}
                       Qt5::Core
                       Qt5::Widgets
                       ${X11_LIBRARIES}
                       -lXfixes
                       -lXcomposite
                       -lXrandr
                       -lXrender )
