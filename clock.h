// Copyright 2012 Eugen Sawin <esawin@me73.com>
#ifndef SRC_CLOCK_H_
#define SRC_CLOCK_H_

#include <cstdint>
#include <ctime>
#include <string>
#include <sstream>

// High-resolution clock used for performance measuring.
// Use this until you have C++11 support via chrono.
class Clock {
 public:
  // Type for time differences.
  typedef int64_t Diff;

  // Clock types used to switch between process, thread and wallclock time.
  enum Type {
    kProcessCpuTime = CLOCK_PROCESS_CPUTIME_ID,
    kThreadCpuTime = CLOCK_THREAD_CPUTIME_ID,
    kRealMonotonic = CLOCK_MONOTONIC,
    kDefType = kProcessCpuTime,
  };

  // Constants used to convert between different time units.
  static const Diff kSecInMin = 60;
  static const Diff kMilliInSec = 1000;
  static const Diff kMicroInMilli = 1000;
  static const Diff kNanoInMicro = 1000;
  static const Diff kMicroInSec = kMilliInSec * kMicroInMilli;
  static const Diff kMicroInMin = kMicroInSec * kSecInMin;
  static constexpr double kMilliInMicro = 1.0 / kMicroInMilli;
  static constexpr double kMicroInNano = 1.0 / kNanoInMicro;
  static constexpr double kSecInMicro = 1.0 / kMicroInSec;
  static constexpr double kMinInMicro = 1.0 / kMicroInMin;

  // Initialised the clock with its default type and the current clock time.
  explicit Clock() : type_(kDefType) {
    clock_gettime(type_, &time_);
  }

  // Initialised the clock with given type and the current clock time.
  explicit Clock(Type type) : type_(type) {
    clock_gettime(type_, &time_);
  }

  // Returns the time difference in microseconds between this and the given
  // clock's time.
  Diff operator-(const Clock& rhs) const {
    return (time_.tv_sec - rhs.time_.tv_sec) * kMicroInSec +
           (time_.tv_nsec - rhs.time_.tv_nsec) * kMicroInNano;
  }

  // Returns the time duration between given times in microseconds.
  static Diff Duration(const Clock& beg, const Clock& end) {
    return end - beg;
  }

  // Returns the string representation of the given time difference.
  static std::string DiffStr(const Diff& diff) {
    std::stringstream ss;
    ss.setf(std::ios::fixed, std::ios::floatfield);
    ss.precision(2);
    if (diff >= kMicroInMin) {
      const double min = diff * kMinInMicro;
      ss << min << "min";
    } else if (diff >= kMicroInSec) {
      const double sec = diff * kSecInMicro;
      ss << sec << "s";
    } else if (diff >= kMicroInMilli) {
      const double milli = diff * kMilliInMicro;
      ss << milli << "ms";
    } else {
      ss << diff << "µs";
    }
    return ss.str();
  }

  // Returns the system time resolution.
  // Remark: Usually returns 0µs (1ns), this is however a bad promise and does
  // not reflect the (dynamic) underlying clock event resolution.
  static Diff Resolution(Type type) {
    timespec res;
    clock_getres(type, &res);
    return res.tv_sec * kMicroInSec + res.tv_nsec * kMicroInNano;
  }

 private:
  Type type_;
  timespec time_;
};

#endif  // SRC_CLOCK_H_

