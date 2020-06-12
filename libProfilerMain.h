#pragma once
#define USE_PROFILER 1
#define LIB_PROFILER_IMPLEMENTATION
#define LIB_PROFILER_PRINTF myPrintf

#include "libProfiler.h"

class LibProfilerMain
{
  public:
    LibProfilerMain()
    {
      PROFILER_ENABLE;
    }
    ~LibProfilerMain()
    {
      LogProfiler();
      PROFILER_DISABLE;
    }
};

LibProfilerMain __lib_profiler_main_instance;
