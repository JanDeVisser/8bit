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

add_library(emucomponents STATIC
        src/cpu/alu.cpp include/alu.h
        src/cpu/backplane.cpp include/backplane.h
        include/clock.h src/cpu/clock.cpp
        include/component.h src/cpu/component.cpp
        include/controller.h src/cpu/controller.cpp
        include/systembus.h src/cpu/systembusview.cpp
        include/harness.h
        include/addressregister.h src/cpu/addressregister.cpp
        include/memory.h src/cpu/memory.cpp
        include/register.h src/cpu/register.cpp
        src/cpu/microcode.inc include/registers.h
        include/opcodes.h)

add_executable(emu
        src/gui/main.cpp

        src/gui/mainwindow.cpp src/gui/mainwindow.h
        src/gui/commands.cpp src/gui/commands.h
        src/gui/cputhread.cpp src/gui/cputhread.h
        src/gui/componentview.cpp src/gui/componentview.h
        src/gui/systembusview.cpp src/gui/systembusview.h
        src/gui/memdump.cpp src/gui/memdump.h

        src/gui/qled.cpp src/gui/qled.h
)

target_link_libraries(
        emu
        PRIVATE
        emucomponents
        Qt${QT_VERSION_MAJOR}::Svg
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Core
)

add_executable(emu_cmdline
        src/cpu/emu.cpp
)

target_link_libraries(emu_cmdline emucomponents)

add_executable(dump_instructions
        src/dump_instructions.cpp include/alu.h include/opcodes.h
)

target_link_libraries(dump_instructions emucomponents)

add_executable(
        emu_test include/opcodes.h
        src/test/clock.cpp include/clock.h
        src/test/register.cpp include/register.h
        src/test/addressregister.cpp include/addressregister.h
        src/test/memory.cpp include/memory.h
        src/test/alu.cpp  include/alu.h
        src/test/controller.cpp include/controller.h src/test/controllertest.h
        src/test/stack.cpp
        src/test/arithmetic.cpp
        src/test/jump.cpp
)

target_link_libraries(emu_test emucomponents ${GTEST_LDFLAGS})
target_compile_options(emu_test PUBLIC ${GTEST_CFLAGS})

include(CTest)
add_test(emu_tests emu_test)


