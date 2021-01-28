cmake_minimum_required(VERSION 3.19)
project(emu VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

cmake_policy(SET CMP0048 NEW)

find_package(PkgConfig)
pkg_search_module(GTEST REQUIRED gtest_main)

include_directories("include")
add_executable(
        emu
        main.cpp
        include/clock.h src/clock.cpp
        include/system.h
        src/lines.cpp include/lines.h
        src/register.cpp include/register.h
        src/addressregister.cpp include/addressregister.h
        src/component.cpp include/component.h
        src/memory.cpp include/memory.h
        src/controller.cpp include/controller.h include/realsystem.h src/realsystem.cpp)

add_executable(
        emu_test
        test/clock.cpp
        include/clock.h src/clock.cpp
        include/system.h
        test/mocksystem.h
        include/addressregister.h src/addressregister.cpp
        include/memory.h src/memory.cpp
        include/register.h src/register.cpp
        test/register.cpp
        test/addressregister.cpp
        test/memory.cpp
)

target_link_libraries(emu_test ${GTEST_LDFLAGS})
target_compile_options(emu_test PUBLIC ${GTEST_CFLAGS})

include(CTest)
add_test(first_and_only_test emu_test)

