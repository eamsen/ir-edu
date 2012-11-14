// Copyright 2012 Eugen Sawin <esawin@me73.com>
#include <random>
#include <algorithm>
#include <functional>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "./intersect.h"
#include "./profiler.h"
#include "./clock.h"

using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::min;

// 0: all off, 1: bold, 4: underscore, 5: blinking, 7: reversed, 8: concealed
// 3x: text, 4x: background
// 0: black, 1: red, 2: green, 3: yellow, 4: blue, 5: magenta, 6: cyan, 7: white
static const char* kResetMode = "\033[0m";
static const char* kBoldText = "\033[1m";
// static const char* kUnderscoreText = "\033[4m";

// Runs the experiments comparing different intersection techniques.
void Experiment(const size_t num_elements, const size_t ratio);

int main(int argc, char* argv[]) {
  // Process command-line arguments.
  vector<string> args(&argv[1], &argv[argc]);
  if (args.size() != 2) {
    cout << "Usage: ./intersect-main <num-elements> <ratio>" << endl;
    return 1;
  }
  // Use double to support scientific notation.
  double num_elements = 0u;
  double ratio = 1u;
  std::stringstream(args[0]) >> num_elements;
  std::stringstream(args[1]) >> ratio;
  // Run the experiment.
  Experiment(num_elements, ratio);
}

// Returns the average duration in microseconds for the execution of given
// function over given number of iterations.
template<typename Func>
Clock::Diff AvgDuration(const Func& func, const size_t num_iter) {
  const auto beg = Clock();
  for (size_t i = 0; i < num_iter; ++i) {
    func();
  }
  return (Clock() - beg) / num_iter;
}

// Returns the duration in microseconds for the execution of given function.
template<typename Func>
Clock::Diff Duration(const Func& func) {
  const auto beg = Clock();
  func();
  return Clock() - beg;
}

// Randomization class.
template<typename It>
class Randomizer {
 public:
  typedef typename std::iterator_traits<It>::value_type value_t;

  // Initializes the randomizer with given range.
  Randomizer(const value_t min, const value_t max)
      : next_(std::bind(std::uniform_int_distribution<value_t>(min, max),
                        std::default_random_engine(std::random_device()()))) {}

  // Randomizes the given sequence uniformally (with duplicates).
  void Randomize(It first, It last) {
    std::generate(first, last, next_);
  }

 private:
  std::function<value_t()> next_;
};

// Basic uniform range-based distribution.
template<typename It>
void DistributeUnif(It first, It last,
                    typename std::iterator_traits<It>::value_type factor) {
  typedef typename std::iterator_traits<It>::value_type value_t;

  value_t value = 0;  // This is ok for all numeric (non-complex) types.
  std::generate(first, last, [&value, factor]() { return value++ * factor; });
}

// Runs the experiment, comparing all intersection versions.
void Experiment(const size_t num_elements, const size_t ratio) {
  cout << "Number of elements: " << static_cast<double>(num_elements)
       << "\nRatio: " << ratio << endl;
  // Generate the lists.
  const size_t list1_size = num_elements / (ratio + 1);
  const size_t list2_size = num_elements - list1_size;
  Randomizer<vector<int>::iterator> randomizer(0u, list2_size);
  vector<int> list1(list1_size);
  auto time1 = Duration([&list1, &randomizer]() {
    // DistributeUnif(list1.begin(), list1.end(), ratio);
    randomizer.Randomize(list1.begin(), list1.end());
  });
  vector<int> list2(list2_size);
  auto time2 = Duration([&list2, &randomizer]() {
    // DistributeUnif(list2.begin(), list2.end(), 1);
    randomizer.Randomize(list2.begin(), list2.end());
  });
  assert(list1.size() + list2.size() == num_elements);
  cout << "Lists construction time: " << Clock::DiffStr(time1 + time2) << endl;
  // Sort the lists.
  time1 = Duration([&list1]() { std::sort(list1.begin(), list1.end()); });
  time2 = Duration([&list2]() { std::sort(list2.begin(), list2.end()); });
  cout << "Lists sorting time: " << Clock::DiffStr(time1 + time2) << endl;

  // Run the experiments, average results over given number of iterations.
  vector<int> reference_result;
  const size_t num_iter = 10u;
  {  // STL intersection.
    reference_result.resize(min(list1.size(), list2.size()));
    vector<int>::iterator end;
    time1 = AvgDuration([&list1, &list2, &reference_result, &end]() {
      end = std::set_intersection(list1.begin(), list1.end(),
                                  list2.begin(), list2.end(),
                                  reference_result.begin());
    }, num_iter);
    cout << "STL set_intersection time: " << kBoldText
         << Clock::DiffStr(time1) << kResetMode << endl;
  }
  {  // Run linear intersection v0
    Profiler::Start("linear-v0.prof");
    vector<int> result;
    time1 = AvgDuration([&list1, &list2, &result]() {
      result = IntersectLin0(list1, list2);
    }, num_iter);
    cout << "Linear intersection v0 time: " << kBoldText
         << Clock::DiffStr(time1) << kResetMode << endl;
    Profiler::Stop();
    assert(reference_result == result);
  }
  {  // Run linear intersection v1
    Profiler::Start("linear-v1.prof");
    vector<int> result;
    time1 = AvgDuration([&list1, &list2, &result]() {
      result = IntersectLin1(list1, list2);
    }, num_iter);
    cout << "Linear intersection v1 time: " << kBoldText
         << Clock::DiffStr(time1) << kResetMode << endl;
    Profiler::Stop();
    assert(reference_result == result);
  }
  {  // Run linear intersection v2
    Profiler::Start("linear-v2.prof");
    vector<int> result;
    time1 = AvgDuration([&list1, &list2, &result]() {
      result = IntersectLin2(list1, list2);
    }, num_iter);
    cout << "Linear intersection v2 time: " << kBoldText
         << Clock::DiffStr(time1) << kResetMode << endl;
    Profiler::Stop();
    assert(reference_result == result);
  }
  {  // Run exponential binary search intersection v0
    Profiler::Start("exponential-v0.prof");
    vector<int> result;
    time1 = AvgDuration([&list1, &list2, &result]() {
      result = IntersectExp0(list1, list2);
    }, num_iter);
    cout << "Exponential intersection v0 time: " << kBoldText
         << Clock::DiffStr(time1) << kResetMode << endl;
    Profiler::Stop();
    assert(reference_result == result);
  }
}
