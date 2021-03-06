///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ___        __       ____                  ___      ___
// /\_ \    __/\ \     /\  _`\              /'___\ __ /\_ \
// \//\ \  /\_\ \ \____\ \ \L\ \_ __   ___ /\ \__//\_\\//\ \      __   _ __
//   \ \ \ \/\ \ \ '__`\\ \ ,__/\`'__\/ __`\ \ ,__\/\ \ \ \ \   /'__`\/\`'__\
//    \_\ \_\ \ \ \ \L\ \\ \ \/\ \ \//\ \L\ \ \ \_/\ \ \ \_\ \_/\  __/\ \ \/
//    /\____\\ \_\ \_,__/ \ \_\ \ \_\\ \____/\ \_\  \ \_\/\____\ \____\\ \_\
//    \/____/ \/_/\/___/   \/_/  \/_/ \/___/  \/_/   \/_/\/____/\/____/ \/_/
//
//
//  Copyright (C) 2007-2013 Cedric Guillemet
//
//  Contact  : cedric.guillemet@gmail.com
//  Twitter  : http://twitter.com/skaven_
//  Web home : http://skaven.fr
//
//  Made with the great help of Christophe Giraud and Maxime Houlier.
//
//    libProfiler is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    libProfiler is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with libProfiler.  If not, see <http://www.gnu.org/licenses/>
//
//
// Changelog:
// 23/12/12 : Initial release
//
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// libProfiler is a tool to measure time taken by a code portion. It can help you improve code
// performance. It can be easily integrated in your toolchain, continuous integration,... It's
// implemented as only one C++ header file! As you define the granularity, it may be more usefull
// than great tools like Verysleepy. And it works well with debug info turned off and full
// optmisation turned on. Sadly, it uses STL (nobody's perfect)
//
// How to use it:
//
// Include this header. For one of .cpp where it's included add:
// #define LIB_PROFILER_IMPLEMENTATION
//
// In your project preprocessor, define USE_PROFILER. If USE_PROFILER is not defined, every
// libProfiler macro is defined empty. So depending on your project target, you can enable/disable
// profiling.
//
// Let's see an example code :
//
//

#ifndef LIBPROFILER_H__
#define LIBPROFILER_H__
//#define LIB_PROFILER_CONFIG_MAIN   //TODO DELETE
#ifdef LIB_PROFILER_CONFIG_MAIN
#define USE_PROFILER 1
#define LIB_PROFILER_IMPLEMENTATION
#define LIB_PROFILER_PRINTF myPrintf
#endif

#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <regex>

//
inline void
myPrintf(const char* szText)
{
  printf("%s", szText);
}
//
//
#define USE_PROFILER 1
// #define LIB_PROFILER_IMPLEMENTATION
#define LIB_PROFILER_PRINTF myPrintf
// #include "libProfiler.h"
//
//
// void myFunction()
// {
//     PROFILER_START(myFunction);
//     float v = 0;
//     for(int i = 0;i<1000000;i++)
//         v += cosf(static_cast<float>(rand()));
//
//     printf("v = %5.4f\n", v);
//     PROFILER_END();
// }
//
// int main(int argc, const char * argv[])
// {
//     PROFILER_ENABLE;
//
//     PROFILER_START(Main);
//
//     std::cout << "Hello, World!\n";
//     myFunction();
//     myFunction();
//
//     PROFILER_END();
//
//     LogProfiler();
//
//     PROFILER_DISABLE;
//
//     return 0;
// }
//
//
//
// Enable/disable profiling with USE_PROFILER.
// In one of your .cpp file, define LIB_PROFILER_IMPLEMENTATION or you'll have troubles linking.
// You can overide the default printf output redefining preprocessor LIB_PROFILER_PRINTF.
// The sample will output:
//
// Hello, World!
// v = -1530.3564
// v = -190.7513
// Profiler:CALLSTACK of Thread 0
// Profiler:_______________________________________________________________________________________
// Profiler:| Total time   | Avg Time     |  Min time    |  Max time    | Calls  | Section
// Profiler:_______________________________________________________________________________________
// Profiler:|      79.0000 |      79.0000 |      79.0000 |      79.0000 |     1  | Main
// Profiler:|      79.0000 |      39.5000 |      38.0000 |      41.0000 |     2  |   myFunction
// Profiler:_______________________________________________________________________________________
//
// Profiler:
//
// Profiler:DUMP of Thread 0
// Profiler:_______________________________________________________________________________________
// Profiler:| Total time   | Avg Time     |  Min time    |  Max time    | Calls  | Section
// Profiler:_______________________________________________________________________________________
// Profiler:|      79.0000 |      79.0000 |      79.0000 |      79.0000 |      1 | Main
// Profiler:|      79.0000 |      39.5000 |      38.0000 |      41.0000 |      2 | myFunction
// Profiler:_______________________________________________________________________________________
//
// The first list correspond to the callstack ( with left spaced function name). You might see a
// a profiled block multiple time depending on where it was called.
// The second list is a flat one. Profiled block only appear once.
// Time unit is ms
//
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// includes

#include <algorithm>
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////////////
// PROFILE/lib_prof_log

#ifndef ProfilerPrintf
#define ProfilerPrintf printf
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
// OS definition

#define PLATFORM_OS_WINDOWS 1
#define PLATFORM_OS_LINUX 2
#define PLATFORM_OS_MACOSX 3

// NOTE: all predefine C/C++ compiler macros: http://sourceforge.net/apps/mediawiki/predef/

#if defined(__WIN32__) || defined(_WIN32) || defined(__WIN64__) || defined(_WIN64) || defined(WIN32)
#define IS_OS_WINDOWS 1
#define IS_OS_LINUX 0
#define IS_OS_MACOSX 0
#define PLATFORM_OS PLATFORM_OS_WINDOWS
//#   pragma message("Platform OS is Windows.")
#elif defined(__linux__) || defined(LINUX)
#define IS_OS_WINDOWS 0
#define IS_OS_LINUX 1
#define IS_OS_MACOSX 0
#define PLATFORM_OS PLATFORM_OS_LINUX
#pragma message("Platform OS is Linux.")
#elif (defined(__APPLE__) && defined(__MACH__)) || defined(MACOSX)
#define IS_OS_WINDOWS 0
#define IS_OS_LINUX 0
#define IS_OS_MACOSX 1
#define PLATFORM_OS PLATFORM_OS_MACOSX
#pragma message("Platform OS is MacOSX.")
#else
#error "This platform is not supported."
#endif

#define PLATFORM_COMPILER_MSVC 1
#define PLATFORM_COMPILER_GCC 2

#if defined(_MSC_VER)
#define PLATFORM_COMPILER PLATFORM_COMPILER_MSVC
#define PLATFORM_COMPILER_VERSION _MSC_VER
#define IS_COMPILER_MSVC 1
#define IS_COMPILER_GCC 0
//#   pragma message("Platform Compiler is Microsoft Visual C++.")
#elif defined(__GNUC__)
#define PLATFORM_COMPILER PLATFORM_COMPILEcmake
#pragma message("Platform Compiler is GCC.")
#else
#error "This compiler is not supported."
#endif

#define PLATFORM_MEMORY_ADDRESS_SPACE_32BIT 1
#define PLATFORM_MEMORY_ADDRESS_SPACE_64BIT 2

#if defined(__x86_64__) || defined(_M_X64) || defined(__powerpc64__)
#define IS_PLATFORM_64BIT 1
#define IS_PLATFORM_32BIT 0
#define PLATFORM_MEMORY_ADDRESS_SPACE PLATFORM_MEMORY_ADDRESS_SPACE_64BIT
//#   pragma message("Using 64bit memory address space.")
#else
#define IS_PLATFORM_64BIT 0
#define IS_PLATFORM_32BIT 1
#define PLATFORM_MEMORY_ADDRESS_SPACE PLATFORM_MEMORY_ADDRESS_SPACE_32BIT
#pragma message("Using 32bit memory address space.")
#endif

#if IS_OS_WINDOWS
#if !defined(snprintf)
#define snprintf sprintf_s
#endif
#endif

#if IS_OS_WINDOWS
#if !defined(vnsprintf)
#define vnsprintf vsprintf_s
#endif
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////

const std::string _NAME_SEPARATOR_ = "|";
const std::string _THREADID_NAME_SEPARATOR_ = "@";

#define _QUOTE(x) #x
#define QUOTE(x) _QUOTE(x)

inline void
lib_prof_log(std::ofstream& output_stream, const char* format, ...)
{
  va_list ptr_arg;
  va_start(ptr_arg, format);

  static char tmps[2048];
  vsprintf(tmps, format, ptr_arg);

  LIB_PROFILER_PRINTF(tmps);
  output_stream << tmps;

  va_end(ptr_arg);
}

inline std::string
get_output_file_name()
{
  std::ostringstream os;
  std::time_t t = std::time(nullptr);
  std::tm tm = *std::localtime(&t);
  os << "lprofile_" << std::put_time(&tm, "%F-%H_%M_%S") << ".txt";
  return os.str();
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// critical section

#if USE_PROFILER

typedef std::mutex ZCriticalSection_t;

__inline ZCriticalSection_t*
NewCriticalSection()
{
  return new std::mutex();
}

__inline void
DestroyCriticalSection(ZCriticalSection_t* cs)
{
  delete cs;
}

__inline void
LockCriticalSection(ZCriticalSection_t* cs)
{
  cs->lock();
}

__inline void
UnLockCriticalSection(ZCriticalSection_t* cs)
{
  cs->unlock();
}

bool
Zprofiler_enable();
void
Zprofiler_disable();
void
Zprofiler_start(const std::string& profile_name);
void
Zprofiler_end();
void
LogProfiler(const double MIN_TIME_TO_REPORT);

class OnLeaveScope
{
public:
  OnLeaveScope() {}
  ~OnLeaveScope() { Zprofiler_end(); }
};

// defines

#define PROFILER_ENABLE Zprofiler_enable()
#define PROFILER_DISABLE Zprofiler_disable()
#define PROFILER_START(x) Zprofiler_start(x)
#define PROFILER_START_F() Zprofiler_start(__FUNCTION__)
#define PROFILER_END() Zprofiler_end()

#if IS_OS_LINUX
#define PROFILER_F()                                                                                                   \
  Zprofiler_start(__PRETTY_FUNCTION__);                                                                                \
  OnLeaveScope __on_leave_scope_call_zprofiler_end;
#elif IS_OS_MACOSX
#define PROFILER_F()                                                                                                   \
  Zprofiler_start(__FUNCTION__);                                                                                       \
  OnLeaveScope __on_leave_scope_call_zprofiler_end;
#elif IS_OS_WINDOWS
#define PROFILER_F()                                                                                                   \
  Zprofiler_start(__FUNCTION__);                                                                                       \
  OnLeaveScope __on_leave_scope_call_zprofiler_end;
#endif

#else

#define LogProfiler()

#define PROFILER_ENABLE
#define PROFILER_DISABLE
#define PROFILER_START(x)
#define PROFILER_END()
#endif

#if USE_PROFILER
#ifdef LIB_PROFILER_IMPLEMENTATION

typedef struct stGenProfilerData
{
  double totalTime = 0.0;
  double ownTime = 0.0;
  double averageTime = 0.0;
  double minTime = 0.0;
  double maxTime = 0.0;
#if IS_OS_LINUX
  std::chrono::system_clock::time_point lastTime; // Time of the previous passage
#elif IS_OS_WINDOWS
  std::chrono::steady_clock::time_point lastTime; // Time of the previous passage
#endif
  double elapsedTime = 0.0;  // Elapsed Time
  unsigned long nbCalls = 0; // Numbers of calls
  std::string callStack;     // temporary.
} tdstGenProfilerData;

//  Hold the call stack
typedef std::vector<tdstGenProfilerData> tdCallStackType;

//  Hold the sequence of profiler_starts
std::map<std::string, tdstGenProfilerData> mapProfilerGraph;

// Hold profiler data vector in function of the thread
std::map<std::thread::id, tdCallStackType> mapCallsByThread;

// Critical section
ZCriticalSection_t* gProfilerCriticalSection;
//
// Activate the profiler
//
bool
Zprofiler_enable()
{
  // Create the mutex
  gProfilerCriticalSection = NewCriticalSection();
  return true;
}

//
// Deactivate the profiler
//
void
Zprofiler_disable()
{
  // Clear maps
  mapCallsByThread.clear();
  mapProfilerGraph.clear();
  mapCallsByThread.clear();

  // Delete the mutex
  DestroyCriticalSection(gProfilerCriticalSection);
}
//
// Start the profiling of a bunch of code
//
void
Zprofiler_start(const std::string& profile_name)
{
  if (gProfilerCriticalSection == nullptr)
    return;

  LockCriticalSection(gProfilerCriticalSection);

  const auto ulThreadId = std::this_thread::get_id();

  // Add the profile name in the callstack vector
  tdstGenProfilerData GenProfilerData;
  GenProfilerData.lastTime = std::chrono::high_resolution_clock::now();
  GenProfilerData.minTime = 0xFFFFFFFF;

  // Find or add callstack
  tdCallStackType TmpCallStack;
  auto IterCallsByThreadMap = mapCallsByThread.find(ulThreadId);
  if (IterCallsByThreadMap == mapCallsByThread.end())
  {
    // Not found. So insert the new pair
    mapCallsByThread.insert(std::make_pair(ulThreadId, TmpCallStack));
    IterCallsByThreadMap = mapCallsByThread.find(ulThreadId);
  }

  // It's the first element of the vector
  if (IterCallsByThreadMap->second.empty())
  {
    GenProfilerData.nbCalls = 1;
    std::stringstream ss;
    ss << ulThreadId << _THREADID_NAME_SEPARATOR_ << profile_name;

    GenProfilerData.callStack = ss.str();
    IterCallsByThreadMap->second.push_back(GenProfilerData);
  }
  // It's not the first element of the vector
  else
  {
    // Update the number of calls
    GenProfilerData.nbCalls++;

    // We need to construct the string with the previous value of the
    // profile_start
    std::string previousString = IterCallsByThreadMap->second[IterCallsByThreadMap->second.size() - 1].callStack;

    // Add the current profile start string
    GenProfilerData.callStack = previousString + _NAME_SEPARATOR_ + profile_name;

    // Push it
    IterCallsByThreadMap->second.push_back(GenProfilerData);
  }

  UnLockCriticalSection(gProfilerCriticalSection);
}

//
// Stop the profiling of a bunch of code
//
void
Zprofiler_end()
{
  if (gProfilerCriticalSection == nullptr)
    return;

  const auto ulThreadId = std::this_thread::get_id();

  // Retrieve the right entry in function of the threadId
  const auto IterCallsByThreadMap = mapCallsByThread.find(ulThreadId);

  // Check if vector is empty
  if (IterCallsByThreadMap->second.empty())
  {
    std::cout << "There is an error in the CallStack vector!!!" << std::endl;
    return;
  }

  LockCriticalSection(gProfilerCriticalSection);

  auto GenProfilerData = IterCallsByThreadMap->second[IterCallsByThreadMap->second.size() - 1];

  // Compute elapsed time
  GenProfilerData.elapsedTime +=
    std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - GenProfilerData.lastTime)
      .count();
  GenProfilerData.totalTime += GenProfilerData.elapsedTime;

  const auto IterMap = mapProfilerGraph.find(GenProfilerData.callStack);
  if (IterMap != mapProfilerGraph.end())
  {
    IterMap->second.nbCalls++;

    // Retrieve time information to compute min and max time
    const auto minTime = IterMap->second.minTime;
    const auto maxTime = IterMap->second.maxTime;

    if (GenProfilerData.elapsedTime < minTime)
    {
      IterMap->second.minTime = GenProfilerData.elapsedTime;
    }

    if (GenProfilerData.elapsedTime > maxTime)
    {
      IterMap->second.maxTime = GenProfilerData.elapsedTime;
    }

    // Compute Total Time
    IterMap->second.totalTime += GenProfilerData.elapsedTime;
    // Compute Average Time
    IterMap->second.averageTime = IterMap->second.totalTime / IterMap->second.nbCalls;
  }
  else
  {
    GenProfilerData.minTime = GenProfilerData.elapsedTime;
    GenProfilerData.maxTime = GenProfilerData.elapsedTime;

    // Compute Average Time
    GenProfilerData.averageTime = GenProfilerData.totalTime / GenProfilerData.nbCalls;

    // Insert this part of the call stack into the Progiler Graph
    mapProfilerGraph.insert(std::make_pair(GenProfilerData.callStack, GenProfilerData));
  }

  // Now, pop back the GenProfilerData from the vector callstack
  IterCallsByThreadMap->second.pop_back();

  UnLockCriticalSection(gProfilerCriticalSection);
}

//
// Dump all data in a file
//

inline bool
MyDataSortPredicate(const tdstGenProfilerData un, const tdstGenProfilerData deux)
{
  return std::string(un.callStack) < std::string(deux.callStack);
}

inline void
LogProfiler(const double total_time_ms)
{
  const double MIN_TIME_TO_REPORT = total_time_ms / 100.;

  // Thread Id String
  std::string szThreadId;
  char textLine[2048];
  std::string tmpString;

  long i;

  // Map for calls
  std::map<std::string, tdstGenProfilerData> mapCalls;
  std::map<std::string, tdstGenProfilerData>::iterator IterMapCalls;
  mapCalls.clear();

  // Map for Thread Ids
  std::map<std::string, int> ThreadIdsCount;
  ThreadIdsCount.clear();

  // Vector for callstack
  std::vector<tdstGenProfilerData> tmpCallStack;
  std::vector<tdstGenProfilerData>::iterator IterTmpCallStack;
  tmpCallStack.clear();

  // Copy map data into a vector in the aim to sort it
  std::map<std::string, tdstGenProfilerData>::iterator IterMap;
  for (IterMap = mapProfilerGraph.begin(); IterMap != mapProfilerGraph.end(); ++IterMap)
  {
    IterMap->second.callStack = IterMap->first;
    tmpCallStack.push_back(IterMap->second);
  }

  // Sort the vector
  std::sort(tmpCallStack.begin(), tmpCallStack.end(), MyDataSortPredicate);

  for (size_t i = 0; i < tmpCallStack.size(); i++)
  {
    double desc_time = 0.0;
    auto call_starts_with = tmpCallStack[i].callStack + '|';

    for (size_t j = i + 1; j < tmpCallStack.size(); j++)
    {
      if (tmpCallStack[j].callStack.compare(0, call_starts_with.length(), call_starts_with) == 0)
      {
        if ( tmpCallStack[j].callStack.find('|', call_starts_with.length()) == std::string::npos)
        {
          desc_time += tmpCallStack[j].totalTime;
        }
      }
      else
      {
        break;
      }
    }
    tmpCallStack[i].ownTime = tmpCallStack[i].totalTime - desc_time;
  }

  // Create a map with thread Ids
  for (IterTmpCallStack = tmpCallStack.begin(); IterTmpCallStack != tmpCallStack.end(); ++IterTmpCallStack)
  {
    auto threadid_name_pos = IterTmpCallStack->callStack.find_first_of(_THREADID_NAME_SEPARATOR_);
    szThreadId = IterTmpCallStack->callStack.substr(0, threadid_name_pos);
    ThreadIdsCount[szThreadId]++;
  }

  // Retrieve the number of thread ids
  // unsigned long nbThreadIds  = mapProfilerGraph.size();
  auto output_file_name = get_output_file_name();
  std::ofstream ofst(output_file_name);

  lib_prof_log(ofst,
               "TOTAL EXECUTION TIME: %9.1f s. MINIMUN TIME TO REPORT: %9.f ms.\n\n",
               total_time_ms / 1000.,
               MIN_TIME_TO_REPORT);

  auto IterThreadIdsCount = ThreadIdsCount.begin();
  for (unsigned long nbThread = 0; nbThread < ThreadIdsCount.size(); nbThread++)
  {
    szThreadId = IterThreadIdsCount->first;

    lib_prof_log(ofst, "CALLSTACK of Thread %s\n", szThreadId.c_str());
    lib_prof_log(ofst,
                 "_________________________________________________________________________________"
                 "____________________\n");
    lib_prof_log(ofst,
                 "| Total time   | Own time     | Avg Time     |  Min time    |  Max time    | "
                 "Calls    | Section\n");
    lib_prof_log(ofst,
                 "_________________________________________________________________________________"
                 "____________________\n");

    for (IterTmpCallStack = tmpCallStack.begin(); IterTmpCallStack != tmpCallStack.end(); ++IterTmpCallStack)
    {
      auto code_name_with_thread = IterTmpCallStack->callStack;
      if (code_name_with_thread.compare(0, szThreadId.length(), szThreadId) == 0)
      {
        auto at_position = code_name_with_thread.find("@");
        auto code_name = code_name_with_thread.substr(at_position + 1);
        // Count the number of separator in the string
        auto nbSeparator = std::count(code_name.begin(), code_name.end(), '|');

        // Get times and fill in the display string
        sprintf(textLine,
                "| %12.1f | %12.1f | %12.1f | %12.1f | %12.1f |%8d  | ",
                IterTmpCallStack->totalTime,
                IterTmpCallStack->ownTime,
                IterTmpCallStack->averageTime,
                IterTmpCallStack->minTime,
                IterTmpCallStack->maxTime,
                static_cast<int>(IterTmpCallStack->nbCalls));

        // Get the last start_profile_name in the string

        auto last_separator = code_name.find_last_of("|");
        if (last_separator != std::string::npos)
        {
          code_name.replace(0, last_separator + 1, "");
        }

        IterMapCalls = mapCalls.find(szThreadId + _THREADID_NAME_SEPARATOR_ + code_name);
        if (IterMapCalls != mapCalls.end())
        {
          const auto minTime = IterMapCalls->second.minTime;
          const auto maxTime = IterMapCalls->second.maxTime;

          if (IterTmpCallStack->minTime < minTime)
          {
            IterMapCalls->second.minTime = IterTmpCallStack->minTime;
          }
          if (IterTmpCallStack->maxTime > maxTime)
          {
            IterMapCalls->second.maxTime = IterTmpCallStack->maxTime;
          }
          IterMapCalls->second.totalTime += IterTmpCallStack->totalTime;
          IterMapCalls->second.ownTime += IterTmpCallStack->ownTime;
          IterMapCalls->second.nbCalls += IterTmpCallStack->nbCalls;
          IterMapCalls->second.averageTime = IterMapCalls->second.totalTime / IterMapCalls->second.nbCalls;
        }
        else
        {
          tdstGenProfilerData tgt;
          tgt.callStack = code_name;
          tgt.minTime = IterTmpCallStack->minTime;
          tgt.maxTime = IterTmpCallStack->maxTime;
          tgt.totalTime = IterTmpCallStack->totalTime;
          tgt.ownTime = IterTmpCallStack->ownTime;
          tgt.averageTime = IterTmpCallStack->averageTime;
          tgt.elapsedTime = IterTmpCallStack->elapsedTime;
          tgt.lastTime = IterTmpCallStack->lastTime;
          tgt.nbCalls = IterTmpCallStack->nbCalls;

          mapCalls.insert(std::make_pair(szThreadId + _THREADID_NAME_SEPARATOR_ + code_name, tgt));
        }

        // Copy white space in the string to format the display
        // in function of the hierarchy
        for (i = 0; i < nbSeparator; i++)
          strcat(textLine, "+");

        // Display the name of the bunch code profiled
        if (IterTmpCallStack->totalTime > MIN_TIME_TO_REPORT)
        {
          lib_prof_log(ofst, "%s%s\n", textLine, code_name.c_str());
        }
      }
    }
    lib_prof_log(ofst,
                 "_________________________________________________________________________________"
                 "____________________\n\n");
    ++IterThreadIdsCount;
  }
  lib_prof_log(ofst, "\n");

  //
  //  DUMP CALLS
  //
  IterThreadIdsCount = ThreadIdsCount.begin();
  for (unsigned long nbThread = 0; nbThread < ThreadIdsCount.size(); nbThread++)
  {
    szThreadId = IterThreadIdsCount->first;

    lib_prof_log(ofst, "DUMP of Thread %s\n", szThreadId.c_str());
    lib_prof_log(ofst,
                 "_________________________________________________________________________________"
                 "____________________\n");
    lib_prof_log(ofst,
                 "| Total time   |  Own Time    |  Avg Time    |  Min time    |  Max time    | "
                 "Calls    | Section\n");
    lib_prof_log(ofst,
                 "_________________________________________________________________________________"
                 "____________________\n");

    for (IterMapCalls = mapCalls.begin(); IterMapCalls != mapCalls.end(); ++IterMapCalls)
    {
      auto code_name_with_thread = IterMapCalls->first;
      if (code_name_with_thread.compare(0, szThreadId.length(), szThreadId) == 0)
      {
        if (IterMapCalls->second.totalTime > MIN_TIME_TO_REPORT)
        {
          lib_prof_log(ofst,
                       "| %12.1f | %12.1f | %12.1f | %12.1f | %12.1f | %8d | %s\n",
                       IterMapCalls->second.totalTime,
                       IterMapCalls->second.ownTime,
                       IterMapCalls->second.averageTime,
                       IterMapCalls->second.minTime,
                       IterMapCalls->second.maxTime,
                       static_cast<int>(IterMapCalls->second.nbCalls),
                       IterMapCalls->second.callStack.c_str());
        }
      }
    }
    lib_prof_log(ofst,
                 "_________________________________________________________________________________"
                 "____________________\n\n");
    ++IterThreadIdsCount;
  }
  ofst.close();
}

#endif // LIB_PROFILER_IMPLEMENTATION

#endif // USE_PROFILER

#ifdef LIB_PROFILER_CONFIG_MAIN

class LibProfilerMain
{
public:
  LibProfilerMain()
  {
    PROFILER_ENABLE;
    _start_time = std::chrono::high_resolution_clock::now();
  }
  ~LibProfilerMain()
  {
    auto total_time_ms =
      std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - _start_time).count();
    LogProfiler(total_time_ms);
    PROFILER_DISABLE;
  }

private:
#if IS_OS_LINUX
  std::chrono::system_clock::time_point _start_time;
#elif IS_OS_WINDOWS
  std::chrono::steady_clock::time_point _start_time;
#endif
};

LibProfilerMain __lib_profiler_main_instance;

#endif

#endif // LIBPROFILER_H__
