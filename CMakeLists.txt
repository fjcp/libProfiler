cmake_minimum_required(VERSION 3.2.1)

project(libProfiler_test LANGUAGES CXX)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
set(CMAKE_CXX_STANDARD 17)

if(UNIX)
  find_package(TBB REQUIRED)
endif()

add_executable(libProfiler_test
  main.cpp
  libProfiler.h
  MyClass.cpp
  MyClass.h
)
if(UNIX)
  target_link_libraries(libProfiler_test tbb)
endif()
