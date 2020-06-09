//
//  main.cpp
//  libProfiler
//
//  Created by Cedric Guillemet on 12/23/12.
//  Copyright (c) 2012 Cedric Guillemet. All rights reserved.
//

#include <iostream>

#define USE_PROFILER 1
#define LIB_PROFILER_IMPLEMENTATION
#define LIB_PROFILER_PRINTF myPrintf
#include "libProfiler.h"
#include "MyClass.h"

int main(int argc, const char * argv[])
{
    PROFILER_ENABLE;

    PROFILER_START_F();

    std::cout << "Hello, World!\n";
    MyClass::myFunction1();
    MyClass::myFunction1();

    PROFILER_END();

    LogProfiler();

    PROFILER_DISABLE;

    return 0;
}

