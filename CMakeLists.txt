cmake_minimum_required(VERSION 3.19)
set(BIN battery_monitor)
project(${BIN})

set(CMAKE_CXX_STANDARD 17)

find_package(Threads)
FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(GTKMM3 REQUIRED gtkmm-3.0)

add_executable(${BIN} main.cpp)

target_include_directories(${BIN} PUBLIC ${GTKMM3_INCLUDE_DIRS})
target_link_directories(${BIN} PUBLIC ${GTKMM3_LIBRARY_DIRS})
target_link_libraries(${BIN} PUBLIC
        Threads::Threads
        ${GTKMM3_LIBRARIES}
        )