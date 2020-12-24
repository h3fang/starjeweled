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

set(BIN_TARGET "terminal")

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(X11 REQUIRED)

add_executable(${BIN_TARGET} ${sources} )
set_target_properties(${BIN_TARGET} PROPERTIES AUTOMOC ON)
target_compile_options(${BIN_TARGET} PRIVATE -Wall -Wextra -pedantic -Werror)
target_include_directories(${BIN_TARGET} PRIVATE ${X11_INCLUDE_DIR} )
target_link_libraries(${BIN_TARGET}
                       Qt5::Core
                       Qt5::Widgets
                       ${X11_LIBRARIES}
                       -lXfixes
                       -lXcomposite
                       -lXrandr
                       -lXrender )
