// Copyright 2012 Eugen Sawin <esawin@me73.com>
#ifndef EXERCISE_SHEET_05_CLOCK_H_
#define EXERCISE_SHEET_05_CLOCK_H_

#include <cstdint>
#include <ctime>
#include <string>
#include <sstream>

// High-resolution clock used for performance measuring.
// Use this until you have C++11 support via chrono.
class Clock {
 public:
  // Type for time differences.
  class Diff {
   public:
    typedef int64_t ValueType;

    // Implicit constructor.
    Diff(const ValueType& value)  // NOLINT
        : value_(value) {}

    // Self-assignment addition.
    Diff& operator+=(const Diff& rhs) {
      value_ += rhs.value_;
      return *this;
    }

    // Self-assignment subtraction.
    Diff& operator-=(const Diff& rhs) {
      value_ -= rhs.value_;
      return *this;
    }

    // Addition.
    Diff operator+(const Diff& rhs) const {
      return Diff(value_ + rhs.value_);
    }

    // Subtraction.
    Diff operator-(const Diff& rhs) const {
      return Diff(value_ - rhs.value_);
    }

    // Division.
    Diff operator/(const Diff& rhs) const {
      return Diff(value_ / rhs.value_);
    }

    // Multiplication.
    Diff operator*(const Diff& rhs) const {
      return Diff(value_ * rhs.value_);
    }

    // Returns the string representation.
    std::string Str() const {
      std::stringstream ss;
      ss.setf(std::ios::fixed, std::ios::floatfield);
      ss.precision(2);
      if (value_ >= kMicroInMin) {
        const double min = value_ * kMinInMicro;
        ss << min << "min";
      } else if (value_ >= kMicroInSec) {
        const double sec = value_ * kSecInMicro;
        ss << sec << "s";
      } else if (value_ >= kMicroInMilli) {
        const double milli = value_ * kMilliInMicro;
        ss << milli << "ms";
      } else {
        ss << value_ << "µs";
      }
      return ss.str();
    }

    const ValueType& value() const {
      return value_;
    }

   private:
    ValueType value_;
  };

  // Clock types used to switch between process, thread and wallclock time.
  enum Type {
    kProcessCpuTime = CLOCK_PROCESS_CPUTIME_ID,
    kThreadCpuTime = CLOCK_THREAD_CPUTIME_ID,
    kRealMonotonic = CLOCK_MONOTONIC,
    kDefType = kProcessCpuTime,
  };

  // Constants used to convert between different time units.
  static const Diff::ValueType kSecInMin = 60;
  static const Diff::ValueType kMilliInSec = 1000;
  static const Diff::ValueType kMicroInMilli = 1000;
  static const Diff::ValueType kNanoInMicro = 1000;
  static const Diff::ValueType kMicroInSec = kMilliInSec * kMicroInMilli;
  static const Diff::ValueType kMicroInMin = kMicroInSec * kSecInMin;
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
    return Diff((time_.tv_sec - rhs.time_.tv_sec) * kMicroInSec +
                (time_.tv_nsec - rhs.time_.tv_nsec) * kMicroInNano);
  }

  // Returns the system time resolution.
  // Remark: Usually returns 0µs (1ns), this is however a bad promise and does
  // not reflect the (dynamic) underlying clock event resolution.
  static Diff Resolution(Type type) {
    timespec res;
    clock_getres(type, &res);
    return Diff(res.tv_sec * kMicroInSec + res.tv_nsec * kMicroInNano);
  }

  // Returns the string representation of the given time difference.
  static std::string DiffStr(const Diff& diff) {
    std::stringstream ss;
    ss.setf(std::ios::fixed, std::ios::floatfield);
    ss.precision(2);
    if (diff.value() >= kMicroInMin) {
      const double min = diff.value() * kMinInMicro;
      ss << min << "min";
    } else if (diff.value() >= kMicroInSec) {
      const double sec = diff.value() * kSecInMicro;
      ss << sec << "s";
    } else if (diff.value() >= kMicroInMilli) {
      const double milli = diff.value() * kMilliInMicro;
      ss << milli << "ms";
    } else {
      ss << diff.value() << "µs";
    }
    return ss.str();
  }

 private:
  Type type_;
  timespec time_;
};

// Stream output operator overload for Diff.
inline std::ostream& operator<<(std::ostream& stream, const Clock::Diff& diff) {
  return stream << diff.Str();
}

#endif  // EXERCISE_SHEET_05_CLOCK_H_
