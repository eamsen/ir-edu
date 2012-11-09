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

template<typename Func>
Clock::Diff AvgDuration(const Func& func, const size_t num_iter) {
  const auto beg = Clock();
  for (size_t i = 0; i < num_iter; ++i) {
    func();
  }
  return (Clock() - beg) / num_iter;
}

template<typename Func>
Clock::Diff Duration(const Func& func) {
  const auto beg = Clock();
  func();
  return Clock() - beg;
}

// Random integer distribution.
template<typename It>
void Randomize(It first, It last) {
  typedef typename std::iterator_traits<It>::value_type value_t;
  // Random number generator function.
  static std::function<value_t()> next =
    std::bind(std::uniform_int_distribution<value_t>(),
              std::default_random_engine(std::random_device()()));

  std::generate(first, last, next);
}

// Uniform range-based distribution.
template<typename It>
void DistributeUnif(It first, It last,
                    typename std::iterator_traits<It>::value_type factor) {
  typedef typename std::iterator_traits<It>::value_type value_t;

  value_t value = 0;  // This is ok for all numeric (non-complex) types.
  std::generate(first, last, [&value, factor]() { return value++ * factor; });
}

void Experiment(const size_t num_elements, const size_t ratio) {
  cout << "Number of elements: " << static_cast<double>(num_elements)
       << "\nRatio: " << ratio << endl;
  // Generate randomized lists.
  vector<int> list1(num_elements / (ratio + 1));
  auto time1 = Duration([&list1, ratio]() {
    DistributeUnif(list1.begin(), list1.end(), ratio);
    // Randomize(list1.begin(), list1.end());
  });
  vector<int> list2(num_elements - list1.size());
  auto time2 = Duration([&list2]() {
    DistributeUnif(list2.begin(), list2.end(), 1);
    // Randomize(list2.begin(), list2.end());
  });
  assert(list1.size() + list2.size() == num_elements);
  cout << "Lists construction time: " << Clock::DiffStr(time1 + time2) << endl;
  // Sort the lists.
  time1 = Duration([&list1]() { std::sort(list1.begin(), list1.end()); });
  time2 = Duration([&list2]() { std::sort(list2.begin(), list2.end()); });
  cout << "Lists sorting time: " << Clock::DiffStr(time1 + time2) << endl;
  // Run the experiments, average results over given number of iterations.
  const size_t num_iter = 10u;
  {  // STL intersection.
    vector<int> result(min(list1.size(), list2.size()));
    vector<int>::iterator end;
    time1 = AvgDuration([&list1, &list2, &result, &end]() {
      end = std::set_intersection(list1.begin(), list1.end(),
                                  list2.begin(), list2.end(), result.begin());
    }, num_iter);
    cout << "STL set_intersection time: " << kBoldText
         << Clock::DiffStr(time1) << kResetMode << endl;
  }
  {  // Run linear intersection v1
    vector<int> result;
    time1 = AvgDuration([&list1, &list2, &result]() {
      result = es::IntersectLin1(list1, list2);
    }, num_iter);
    cout << "Linear intersection v1 time: " << kBoldText
         << Clock::DiffStr(time1) << kResetMode << endl;
  }
  {  // Run linear intersection v2
    vector<int> result;
    time1 = AvgDuration([&list1, &list2, &result]() {
      result = es::IntersectLin2(list1, list2);
    }, num_iter);
    cout << "Linear intersection v2 time: " << kBoldText
         << Clock::DiffStr(time1) << kResetMode << endl;
  }
}
