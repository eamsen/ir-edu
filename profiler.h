// Copyright 2012 Eugen Sawin <esawin@me73.com>
#ifndef SRC_PROFILER_H_
#define SRC_PROFILER_H_

#ifdef PROFILE
  #include <gperftools/profiler.h>
#endif  // PROFILE

#include <string>
 
// Namespace for the profiling commands.
struct Profiler {
  // Starts the profiling process, writes stats to file at given path.
  static void Start(const std::string& path) {
#ifdef PROFILE
    ProfilerStart(path.c_str());
#endif  // PROFILE
  }
 
  // Stops the profiling process and finalises the stats.
  static void Stop() {
#ifdef PROFILE
    ProfilerStop();
#endif  // PROFILE
  }
};
 
#endif  // SRC_PROFILER_H_
