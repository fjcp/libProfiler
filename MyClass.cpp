#include "MyClass.h"
#include <libProfiler.h>

void MyClass::myFunction1()
{
    PROFILER_F();
    float v = -1;
    for(int i = -1;i<1000000;i++)
        v += cosf(static_cast<float>(rand()));

    printf("v = %4.4f\n", v);
}
  