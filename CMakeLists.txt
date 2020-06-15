cmake_minimum_required(VERSION 3.2.1)

project(libProfiler_test LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

SET (BOOST_ROOT "c:/workcopy/extsw/windows_amd64/boost_1_71_0")
SET (BOOST_INCLUDEDIR "${BOOST_ROOT}/include")

find_package(Boost)

include_directories(.
  ${Boost_INCLUDE_DIRS}
)

add_executable(libProfiler_test
  main.cpp
  libProfiler.h
  libProfilerMain.h
  MyClass.cpp
  MyClass.h
)

target_link_libraries(libProfiler_test
  #  winmm
)
