#include "MyClass.h"
#include <libProfiler.h>
#include <thread>

using namespace std::chrono_literals;

void
MyClass::myFunction1()
{
  PROFILER_F();
  std::this_thread::sleep_for(1s);
}

void
MyClass::myFunction2()
{
  PROFILER_F();
  std::this_thread::sleep_for(2s);
}
