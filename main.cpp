//
//  main.cpp
//  libProfiler
//
//  Created by Cedric Guillemet on 12/23/12.
//  Copyright (c) 2012 Cedric Guillemet. All rights reserved.

#define LIB_PROFILER_CONFIG_MAIN

#include "libProfiler.h"
//
#include "MyClass.h"
#include <algorithm>
#include <execution>
#include <iostream>

int
main(int argc, const char* argv[])
{
  PROFILER_F();

  std::cout << "hello, world!\n";
  MyClass::myFunction1();

  std::vector<int> v{ 0, 1, 2 };

  std::for_each(v.begin(), v.end(), [](const auto& item) { MyClass::myFunction1(); });

  MyClass::myFunction2();

  std::for_each(
    std::execution::par, v.begin(), v.end(), [](const auto& item) { MyClass::myFunction2(); });

  return 0;
}
