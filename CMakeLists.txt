cmake_minimum_required(VERSION 3.16)
project(AppliGPS VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Quick QuickControls2 Positioning Location Network Test)

set(SOURCES
    src/WaypointModel.cpp
    src/GpxHandler.cpp
    src/RouteEngine.cpp
    src/GpsProvider.cpp
    src/LanguageManager.cpp
    src/NavigationController.cpp
)

set(HEADERS
    src/Waypoint.h
    src/WaypointModel.h
    src/GpxHandler.h
    src/RouteEngine.h
    src/GpsProvider.h
    src/LanguageManager.h
    src/NavigationController.h
)

# Main Application Executable
add_executable(AppliGPS
    src/main.cpp
    ${SOURCES}
    ${HEADERS}
    qml.qrc
)

target_link_libraries(AppliGPS PRIVATE
    Qt5::Core
    Qt5::Quick
    Qt5::QuickControls2
    Qt5::Positioning
    Qt5::Location
    Qt5::Network
)

# Automated Unit Tests
enable_testing()

add_executable(AppliGPS_tests
    tests/test_navigation.cpp
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(AppliGPS_tests PRIVATE
    Qt5::Core
    Qt5::Positioning
    Qt5::Test
)

add_test(NAME AppliGPS_tests COMMAND AppliGPS_tests)
