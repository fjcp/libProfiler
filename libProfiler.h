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

#ifdef LIB_PROFILER_CONFIG_MAIN
#define USE_PROFILER 1
#define LIB_PROFILER_IMPLEMENTATION
#define LIB_PROFILER_PRINTF myPrintf
#endif

//#include <boost/algorithm/string.hpp>
//#include <boost/scope_exit.hpp>
#include <regex>
//
inline void
myPrintf(const char* szText)
{
  printf("Profiler:%s", szText);
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
#include <cstdarg>
#include <cstdio>
#include <map>
#include <string>
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
#define PLATFORM_COMPILER PLATFORM_COMPILER_GCC
#define PLATFORM_COMPILER_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100)
#define IS_COMPILER_MSVC 0
#define IS_COMPILER_GCC 1
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

const std::string  _NAME_SEPARATOR_ = "|";
const std::string _THREADID_NAME_SEPARATOR_ = "@";

#define _QUOTE(x) #x
#define QUOTE(x) _QUOTE(x)

inline void
lib_prof_log(const char* format, ...)
{
  va_list ptr_arg;
  va_start(ptr_arg, format);

  static char tmps[2048];
  vsprintf(tmps, format, ptr_arg);

  LIB_PROFILER_PRINTF(tmps);

  va_end(ptr_arg);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// critical section

#if USE_PROFILER

// Critical Section
#if IS_OS_WINDOWS
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
typedef CRITICAL_SECTION ZCriticalSection_t;

#elif IS_OS_LINUX
#include <pthread.h>
typedef pthread_mutex_t ZCriticalSection_t;

#elif IS_OS_MACOSX
#import <CoreServices/CoreServices.h>
typedef MPCriticalRegionID ZCriticalSection_t;
inline char*
ZGetCurrentDirectory(int bufLength, char* pszDest)
{
  return getcwd(pszDest, bufLength);
}
#endif

__inline ZCriticalSection_t*
NewCriticalSection()
{
#if IS_OS_LINUX
  ZCriticalSection_t* cs = new pthread_mutex_t;
  pthread_mutex_init(cs, NULL);
  return cs;
#elif IS_OS_MACOSX
  MPCriticalRegionID* criticalRegion = new MPCriticalRegionID;
  OSStatus err = MPCreateCriticalRegion(criticalRegion);
  if (err != 0)
  {
    delete criticalRegion;
    criticalRegion = NULL;
  }

  return criticalRegion;
#elif IS_OS_WINDOWS
  CRITICAL_SECTION* cs = new CRITICAL_SECTION;
  InitializeCriticalSection(cs);
  return cs;
#endif
}

__inline void
DestroyCriticalSection(ZCriticalSection_t* cs)
{
#if IS_OS_LINUX
  delete cs;
#elif IS_OS_MACOSX
  MPDeleteCriticalRegion(*cs);
#elif IS_OS_WINDOWS
  DeleteCriticalSection(cs);
  delete cs;
#endif
}

__inline void
LockCriticalSection(ZCriticalSection_t* cs)
{
#if IS_OS_LINUX
  pthread_mutex_lock(cs);
#elif IS_OS_MACOSX
  MPEnterCriticalRegion(*cs, kDurationForever);
#elif IS_OS_WINDOWS
  EnterCriticalSection(cs);
#endif
}

__inline void
UnLockCriticalSection(ZCriticalSection_t* cs)
{
#if IS_OS_LINUX
  pthread_mutex_unlock(cs);
#elif IS_OS_MACOSX
  MPExitCriticalRegion(*cs);
#elif IS_OS_WINDOWS
  LeaveCriticalSection(cs);
#endif
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
LogProfiler();


class OnLeaveScope
{
public:
  OnLeaveScope() {}
  ~OnLeaveScope()
  {
    Zprofiler_end();
  }
};

// defines

#define PROFILER_ENABLE Zprofiler_enable()
#define PROFILER_DISABLE Zprofiler_disable()
#define PROFILER_START(x) Zprofiler_start(x)
#define PROFILER_START_F() Zprofiler_start(__FUNCTION__)
#define PROFILER_END() Zprofiler_end()

#define PROFILER_F()                                                                               \
  Zprofiler_start(__FUNCTION__);                                                                   \
  OnLeaveScope __on_leave_scope_call_zprofiler_end;

#else

#define LogProfiler()

#define PROFILER_ENABLE
#define PROFILER_DISABLE
#define PROFILER_START(x)
#define PROFILER_END()
#endif

#if USE_PROFILER
#ifdef LIB_PROFILER_IMPLEMENTATION

using namespace std;

void
TimerInit();
//#if defined(WIN32)
double
startHighResolutionTimer(void);

#if IS_OS_WINDOWS
// Create A Structure For The Timer Information
struct
{
  __int64 frequency;                 // Timer Frequency
  float resolution;                  // Timer Resolution
  unsigned long mm_timer_start;      // Multimedia Timer Start Value
  unsigned long mm_timer_elapsed;    // Multimedia Timer Elapsed Time
  bool performance_timer;            // Using The Performance Timer?
  __int64 performance_timer_start;   // Performance Timer Start Value
  __int64 performance_timer_elapsed; // Performance Timer Elapsed Time
} timer;                             // Structure Is Named timer

#endif //  IS_OS_WINDOWS

typedef struct stGenProfilerData
{
  double totalTime = 0.0;
  double averageTime = 0.0;
  double minTime = 0.0;
  double maxTime = 0.0;
  double lastTime = 0.0;            // Time of the previous passage
  double elapsedTime = 0.0;         // Elapsed Time
  unsigned long nbCalls=0;      // Numbers of calls
  std::string szBunchCodeName; // temporary.
} tdstGenProfilerData;

//  Hold the call stack
typedef std::vector<tdstGenProfilerData> tdCallStackType;

//  Hold the sequence of profiler_starts
std::map<std::string, tdstGenProfilerData> mapProfilerGraph;

// Hold profiler data vector in function of the thread
map<unsigned long, tdCallStackType> mapCallsByThread;

// Critical section
ZCriticalSection_t* gProfilerCriticalSection;
//
// Activate the profiler
//
bool
Zprofiler_enable()
{
  // Initialize the timer
  TimerInit();

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
  // Dump to file

  // Clear maps
  mapCallsByThread.clear();
  mapProfilerGraph.clear();
  mapCallsByThread.clear();

  // Delete the mutex
  DestroyCriticalSection(gProfilerCriticalSection);
}
#if IS_OS_MACOSX
unsigned long
GetCurrentThreadId()
{
  return 0;
}
#elif IS_OS_LINUX
unsigned long
GetCurrentThreadId()
{
  return pthread_self();
}
#endif

//
// Start the profiling of a bunch of code
//
void
Zprofiler_start(const std::string& profile_name)
{
  if (gProfilerCriticalSection == nullptr)
    return;

  LockCriticalSection(gProfilerCriticalSection);

  unsigned long ulThreadId = GetCurrentThreadId();

  // Add the profile name in the callstack vector
  tdstGenProfilerData GenProfilerData;
//  memset(&GenProfilerData, 0, sizeof(GenProfilerData));
  GenProfilerData.lastTime = startHighResolutionTimer();
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
  if ((*IterCallsByThreadMap).second.empty())
  {
    GenProfilerData.nbCalls = 1;
    GenProfilerData.szBunchCodeName = std::to_string(ulThreadId) + _THREADID_NAME_SEPARATOR_ + profile_name;
/*    sprintf(GenProfilerData.szBunchCodeName,
            "%s%d%s%s",
            _NAME_SEPARATOR_,
            (int)ulThreadId,
            _THREADID_NAME_SEPARATOR_,
            profile_name.c_str()); 
            */
    (*IterCallsByThreadMap).second.push_back(GenProfilerData);
  }
  // It's not the first element of the vector
  else
  {
    // Update the number of calls
    GenProfilerData.nbCalls++;

    // We need to construct the string with the previous value of the
    // profile_start
    std::string previousString =
      (*IterCallsByThreadMap).second[(*IterCallsByThreadMap).second.size() - 1].szBunchCodeName;

    // Add the current profile start string
    GenProfilerData.szBunchCodeName =  previousString + _NAME_SEPARATOR_ + profile_name;

    // Push it
    (*IterCallsByThreadMap).second.push_back(GenProfilerData);
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

  const unsigned long ulThreadId = GetCurrentThreadId();

  // Retrieve the right entry in function of the threadId
  const auto IterCallsByThreadMap = mapCallsByThread.find(ulThreadId);

  // Check if vector is empty
  if ((*IterCallsByThreadMap).second.empty())
  {
    lib_prof_log("Il y a une erreur dans le vecteur CallStack !!!\n\n");
    return;
  }

  LockCriticalSection(gProfilerCriticalSection);

  auto GenProfilerData = (*IterCallsByThreadMap).second[(*IterCallsByThreadMap).second.size() - 1];

  // Compute elapsed time
  GenProfilerData.elapsedTime += startHighResolutionTimer() - GenProfilerData.lastTime;
  GenProfilerData.totalTime += GenProfilerData.elapsedTime;

  const auto IterMap = mapProfilerGraph.find(GenProfilerData.szBunchCodeName);
  if (IterMap != mapProfilerGraph.end())
  {
    (*IterMap).second.nbCalls++;

    // Retrieve time information to compute min and max time
    const auto minTime = (*IterMap).second.minTime;
    const auto maxTime = (*IterMap).second.maxTime;

    if (GenProfilerData.elapsedTime < minTime)
    {
      (*IterMap).second.minTime = GenProfilerData.elapsedTime;
    }

    if (GenProfilerData.elapsedTime > maxTime)
    {
      (*IterMap).second.maxTime = GenProfilerData.elapsedTime;
    }

    // Compute Total Time
    (*IterMap).second.totalTime += GenProfilerData.elapsedTime;
    // Compute Average Time
    (*IterMap).second.averageTime = (*IterMap).second.totalTime / (*IterMap).second.nbCalls;
  }
  else
  {
    GenProfilerData.minTime = GenProfilerData.elapsedTime;
    GenProfilerData.maxTime = GenProfilerData.elapsedTime;

    // Compute Average Time
    GenProfilerData.averageTime = GenProfilerData.totalTime / GenProfilerData.nbCalls;

    // Insert this part of the call stack into the Progiler Graph
    mapProfilerGraph.insert(std::make_pair(GenProfilerData.szBunchCodeName, GenProfilerData));
  }

  // Now, pop back the GenProfilerData from the vector callstack
  (*IterCallsByThreadMap).second.pop_back();

  UnLockCriticalSection(gProfilerCriticalSection);
}

//
// Dump all data in a file
//

inline bool
MyDataSortPredicate(const tdstGenProfilerData un, const tdstGenProfilerData deux)
{
  return std::string(un.szBunchCodeName) < std::string(deux.szBunchCodeName);
}

inline void
LogProfiler()
{

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
  vector<tdstGenProfilerData> tmpCallStack;
  vector<tdstGenProfilerData>::iterator IterTmpCallStack;
  tmpCallStack.clear();

  // Copy map data into a vector in the aim to sort it
  std::map<std::string, tdstGenProfilerData>::iterator IterMap;
  for (IterMap = mapProfilerGraph.begin(); IterMap != mapProfilerGraph.end(); ++IterMap)
  {
    (*IterMap).second.szBunchCodeName =  (*IterMap).first;
    tmpCallStack.push_back((*IterMap).second);
  }

  // Sort the vector
  std::sort(tmpCallStack.begin(), tmpCallStack.end(), MyDataSortPredicate);

  auto it_max =
    std::max_element(tmpCallStack.begin(),
                     tmpCallStack.end(),
                     [](const tdstGenProfilerData& data1, const tdstGenProfilerData& data2) {
                       return data1.totalTime < data2.totalTime;
                     });

  const auto MAX_TOTAL_TIME = it_max->totalTime;

  // Create a map with thread Ids
  for (IterTmpCallStack = tmpCallStack.begin(); IterTmpCallStack != tmpCallStack.end();
       ++IterTmpCallStack)
  {
    auto threadid_name_pos = IterTmpCallStack->szBunchCodeName.find_first_of(_THREADID_NAME_SEPARATOR_);
    szThreadId = IterTmpCallStack->szBunchCodeName.substr(0, threadid_name_pos);
    ThreadIdsCount[szThreadId]++;
  }

  // Retrieve the number of thread ids
  // unsigned long nbThreadIds  = mapProfilerGraph.size();

  auto IterThreadIdsCount = ThreadIdsCount.begin();
  for (unsigned long nbThread = 0; nbThread < ThreadIdsCount.size(); nbThread++)
  {
    szThreadId =  IterThreadIdsCount->first;

    lib_prof_log("CALLSTACK of Thread %s\n", szThreadId.c_str());
    lib_prof_log(
      "_______________________________________________________________________________________\n");
    lib_prof_log(
      "| Total time   | Avg Time     |  Min time    |  Max time    | Calls  | Section\n");
    lib_prof_log(
      "_______________________________________________________________________________________\n");

    for (IterTmpCallStack = tmpCallStack.begin(); IterTmpCallStack != tmpCallStack.end();
         ++IterTmpCallStack)
    {
      auto code_name_with_thread = IterTmpCallStack->szBunchCodeName;
      if( code_name_with_thread.compare(0, szThreadId.length(), szThreadId) == 0)
      {
        auto at_position = code_name_with_thread.find("@");
        auto code_name = code_name_with_thread.substr(at_position + 1);
        // Count the number of separator in the string
        auto nbSeparator = std::count(code_name.begin(), code_name.end(), '|');

        // Get times and fill in the display string
        sprintf(textLine,
                "| %12.4f | %12.4f | %12.4f | %12.4f |%6d  | ",
                (*IterTmpCallStack).totalTime,
                (*IterTmpCallStack).averageTime,
                (*IterTmpCallStack).minTime,
                (*IterTmpCallStack).maxTime,
                static_cast<int>((*IterTmpCallStack).nbCalls));

        // Get the last start_profile_name in the string

        auto last_separator = code_name.find_last_of("|");
        if (last_separator != std::string::npos)
        {
          code_name.replace(0, last_separator+1, "");
        }

        IterMapCalls = mapCalls.find(szThreadId+_THREADID_NAME_SEPARATOR_+ code_name);
        if (IterMapCalls != mapCalls.end())
        {
          const auto minTime = (*IterMapCalls).second.minTime;
          const auto maxTime = (*IterMapCalls).second.maxTime;

          if ((*IterTmpCallStack).minTime < minTime)
          {
            (*IterMapCalls).second.minTime = (*IterTmpCallStack).minTime;
          }
          if ((*IterTmpCallStack).maxTime > maxTime)
          {
            (*IterMapCalls).second.maxTime = (*IterTmpCallStack).maxTime;
          }
          (*IterMapCalls).second.totalTime += (*IterTmpCallStack).totalTime;
          (*IterMapCalls).second.nbCalls += (*IterTmpCallStack).nbCalls;
          (*IterMapCalls).second.averageTime =
            (*IterMapCalls).second.totalTime / (*IterMapCalls).second.nbCalls;
        }
        else
        {
          tdstGenProfilerData tgt;
          tgt.szBunchCodeName = code_name;
          tgt.minTime = (*IterTmpCallStack).minTime;
          tgt.maxTime = (*IterTmpCallStack).maxTime;
          tgt.totalTime = (*IterTmpCallStack).totalTime;
          tgt.averageTime = (*IterTmpCallStack).averageTime;
          tgt.elapsedTime = (*IterTmpCallStack).elapsedTime;
          tgt.lastTime = (*IterTmpCallStack).lastTime;
          tgt.nbCalls = (*IterTmpCallStack).nbCalls;

          mapCalls.insert(std::make_pair(szThreadId + _THREADID_NAME_SEPARATOR_ + code_name, tgt));
        }

        // Copy white space in the string to format the display
        // in function of the hierarchy
        for (i = 0; i < nbSeparator; i++)
          strcat(textLine, "+");

        // Display the name of the bunch code profiled
        if (IterTmpCallStack->totalTime > MAX_TOTAL_TIME / 100.)
        {
          lib_prof_log("%s%s\n", textLine, code_name.c_str());
        }
      }
    }
    lib_prof_log(
      "_______________________________________________________________________________________"
      "\n\n");
    ++IterThreadIdsCount;
  }
  lib_prof_log("\n\n");

  //
  //  DUMP CALLS
  //
  IterThreadIdsCount = ThreadIdsCount.begin();
  for (unsigned long nbThread = 0; nbThread < ThreadIdsCount.size(); nbThread++)
  {
    szThreadId =  IterThreadIdsCount->first;

    lib_prof_log("DUMP of Thread %s\n", szThreadId.c_str());
    lib_prof_log(
      "_______________________________________________________________________________________\n");
    lib_prof_log(
      "| Total time   | Avg Time     |  Min time    |  Max time    | Calls  | Section\n");
    lib_prof_log(
      "_______________________________________________________________________________________\n");

    for (IterMapCalls = mapCalls.begin(); IterMapCalls != mapCalls.end(); ++IterMapCalls)
    {
      auto code_name_with_thread = (*IterMapCalls).first;
      if( code_name_with_thread.compare(0, szThreadId.length(), szThreadId) == 0)
      { 
        if ((*IterMapCalls).second.totalTime > MAX_TOTAL_TIME / 100.)
        {
          lib_prof_log("| %12.4f | %12.4f | %12.4f | %12.4f | %6d | %s\n",
                        (*IterMapCalls).second.totalTime,
                        (*IterMapCalls).second.averageTime,
                        (*IterMapCalls).second.minTime,
                        (*IterMapCalls).second.maxTime,
                        static_cast<int>((*IterMapCalls).second.nbCalls),
                        (*IterMapCalls).second.szBunchCodeName.c_str());
        }
      }
    }
    lib_prof_log(
      "_______________________________________________________________________________________"
      "\n\n");
    ++IterThreadIdsCount;
  }
}

////
////  Gestion des timers
////
#if IS_OS_WINDOWS

// Initialize Our Timer (Get It Ready)
inline void
TimerInit()
{
  memset(&timer, 0, sizeof(timer));

  // Check to see if a performance counter is available
  // If one is available the timer frequency will be updated
  QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&timer.frequency));
  // Performance counter is available, use it instead of the multimedia timer
  // Get the current time and store it in performance_timer_start
  QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&timer.performance_timer_start));
  timer.performance_timer = true; // Set performance timer to true

  // Calculate the timer resolution using the timer frequency
  timer.resolution =
    static_cast<float>(static_cast<double>(1.0f) / static_cast<double>(timer.frequency));

  // Set the elapsed time to the current time
  timer.performance_timer_elapsed = timer.performance_timer_start;
}

// platform specific get hires times...
inline double
startHighResolutionTimer()
{
  __int64 time;
  // Grab the current performance time
  QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&time));

  // Return the current time minus the start time multiplied
  // by the resolution and 1000 (To Get MS)
  return (static_cast<double>(time - timer.performance_timer_start) * timer.resolution) * 1000.0f;
}
#elif IS_OS_MACOSX

// Initialize Our Timer (Get It Ready)
void
TimerInit()
{
}

double
startHighResolutionTimer()
{
  UnsignedWide t;
  Microseconds(&t);
  /*time[0] = t.lo;
   time[1] = t.hi;
   */
  double ms = double(t.hi * 1000LU);
  ms += double(t.lo / 1000LU); //*0.001;

  return ms;
}
#else

// Initialize Our Timer (Get It Ready)
void
TimerInit()
{
}

double
startHighResolutionTimer()
{
  timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts); // Works on Linux

  double ms = double(ts.tv_sec * 1000LU);
  ms += double(ts.tv_nsec / 1000LU) * 0.001;

  return ms;
}
#endif

#endif // LIB_PROFILER_IMPLEMENTATION

#endif // USE_PROFILER

#ifdef LIB_PROFILER_CONFIG_MAIN

class LibProfilerMain
{
public:
  LibProfilerMain() { PROFILER_ENABLE; }
  ~LibProfilerMain()
  {
    LogProfiler();
    PROFILER_DISABLE;
  }
};

LibProfilerMain __lib_profiler_main_instance;

#endif

#endif // LIBPROFILER_H__
