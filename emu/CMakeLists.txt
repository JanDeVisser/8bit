cmake_minimum_required(VERSION 3.5)

project(emu VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_policy(SET CMP0048 NEW)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(PkgConfig)
pkg_search_module(GTEST REQUIRED gtest_main)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Svg REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Svg REQUIRED)
include_directories("include")

add_executable(emu
        src/gui/main.cpp
        src/gui/mainwindow.cpp
        src/gui/mainwindow.h
        include/clock.h src/cpu/clock.cpp
        include/backplane.h src/cpu/backplane.cpp
        src/cpu/systembus.cpp include/systembus.h
        src/cpu/register.cpp include/register.h
        src/cpu/addressregister.cpp include/addressregister.h
        src/cpu/component.cpp include/component.h
        src/cpu/memory.cpp include/memory.h
        src/cpu/controller.cpp include/controller.h
        src/gui/qled.cpp src/gui/qled.h src/gui/cputhread.h src/gui/cputhread.cpp src/gui/componentview.h src/gui/componentview.cpp src/cpu/microcode.inc include/registers.h)

target_link_libraries(
        emu
        PRIVATE
        Qt${QT_VERSION_MAJOR}::Svg
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Core
)

add_executable(emu_cmdline
        src/cpu/emu.cpp
        include/clock.h src/cpu/clock.cpp
        include/backplane.h src/cpu/backplane.cpp
        src/cpu/systembus.cpp include/systembus.h
        src/cpu/register.cpp include/register.h
        src/cpu/addressregister.cpp include/addressregister.h
        src/cpu/component.cpp include/component.h
        src/cpu/memory.cpp include/memory.h
        src/cpu/controller.cpp include/controller.h
        src/cpu/microcode.inc include/registers.h)

add_executable(
    emu_test
    src/test/clock.cpp
    include/clock.h src/cpu/clock.cpp
    include/component.h src/cpu/component.cpp
    include/controller.h src/cpu/controller.cpp
    include/systembus.h src/cpu/systembus.cpp
    include/harness.h
    include/addressregister.h src/cpu/addressregister.cpp
    include/memory.h src/cpu/memory.cpp
    include/register.h src/cpu/register.cpp
    src/test/clock.cpp
    src/test/register.cpp
    src/test/addressregister.cpp
    src/test/memory.cpp
    src/test/zzcontroller.cpp
    src/cpu/microcode.inc include/registers.h
)

target_link_libraries(emu_test ${GTEST_LDFLAGS})
target_compile_options(emu_test PUBLIC ${GTEST_CFLAGS})

include(CTest)
add_test(first_and_only_test emu_test)


