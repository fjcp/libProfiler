#pragma once
#define USE_PROFILER 1
#define LIB_PROFILER_IMPLEMENTATION
#define LIB_PROFILER_PRINTF myPrintf

#include <boost/scope_exit.hpp> 
#include "libProfiler.h"

#define PROFILER_MAIN() \
  PROFILER_ENABLE; \
  PROFILER_START_F(); \
  Zprofiler_start(__FUNCTION__);    \
  BOOST_SCOPE_EXIT(void){ \
    PROFILER_END(); \
    LogProfiler(); \
    PROFILER_DISABLE; \
  } BOOST_SCOPE_EXIT_END

