//
//  main.cpp
//  libProfiler
//
//  Created by Cedric Guillemet on 12/23/12.
//  Copyright (c) 2012 Cedric Guillemet. All rights reserved.
//
#include "libProfilerMain.h"
#include <iostream>
#include "MyClass.h"

int main(int argc, const char * argv[])
{
  PROFILER_F();

  std::cout << "hello, world!\n";
  MyClass::myFunction1();
  MyClass::myFunction1();

  return 0;
}

