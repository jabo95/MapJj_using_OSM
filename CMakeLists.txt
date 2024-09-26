cmake_minimum_required(VERSION 3.28)
project(usingOSM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)


find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(usingOSM main.cpp
        Widget.cpp
        Widget.h)
target_link_libraries(usingOSM
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
)

