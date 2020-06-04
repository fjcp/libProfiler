project(libProfiler_test)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_executable(libProfiler_test main.cpp)

target_link_libraries(libProfiler_test
  winmm
  )
