// Copyright 2012 Eugen Sawin <esawin@me73.com>
#include <random>
#include <functional>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <limits>
#include <queue>
#include "./intersect.h"
#include "./profiler.h"
#include "./clock.h"

using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::bind;
using std::cref;

// 0: all off, 1: bold, 4: underscore, 5: blinking, 7: reversed, 8: concealed
// 3x: text, 4x: background
// 0: black, 1: red, 2: green, 3: yellow, 4: blue, 5: magenta, 6: cyan, 7: white
static const char* kResetMode = "\033[0m";
static const char* kBoldText = "\033[1m";
static const char* kUnderscoreText = "\033[4m";

void Experiment(const size_t num_elements, const size_t ratio);
vector<int> RandomList(const size_t num_elements);

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

template<typename Func, typename T>
Clock::Diff Duration(const Func& func, T* ret) {
  assert(ret);
  const auto beg = Clock();
  *ret = func();
  return Clock() - beg;
}

template<typename Func>
Clock::Diff Duration(const Func& func) {
  const auto beg = Clock();
  func();
  return Clock() - beg;
}

template<typename It>
void Randomize(It first, It last) {
  typedef typename std::iterator_traits<It>::value_type value_type;
  // Random number generator function.
  static std::function<value_type()> next =
    bind(std::uniform_int_distribution<value_type>(),
         std::default_random_engine(std::random_device()()));

  std::generate(first, last, next);
}

void Experiment(const size_t num_elements, const size_t ratio) {
  cout << "Number of elements: " << static_cast<double>(num_elements)
       << "\nRatio: " << ratio;
  // Generate randomized lists.
  vector<int> list1(num_elements / (ratio + 1));
  auto func = bind(Randomize<vector<int>::iterator>,
                   list1.begin(), list1.end());
  auto time1 = Duration(func);
  vector<int> list2(num_elements - list1.size());
  func = bind(Randomize<vector<int>::iterator>, list2.begin(), list2.end());
  auto time2 = Duration(func);
  assert(list1.size() + list2.size() == num_elements);
  cout << "\nLists construction time: " << Clock::DiffStr(time1 + time2);
  // Sort the lists.
  func = bind(std::sort<vector<int>::iterator>, list1.begin(), list1.end());
  time1 = Duration(func);
  func = bind(std::sort<vector<int>::iterator>, list2.begin(), list2.end());
  time2 = Duration(func);
  cout << "\nLists sorting time: " << Clock::DiffStr(time1 + time2);
  // Run linear intersection.
  vector<int> result;
  auto func2 = bind(es::IntersectLin<vector<int> >, cref(list1), cref(list2));
  for (int i = 0; i < 5; ++i) {
    time1 = Duration(func2, &result);
    cout << "\nLinear intersection time (" << i + 1 << "): "
         << Clock::DiffStr(time1);
  }

  cout << "\nResult: ";
  std::for_each(result.begin(), result.begin() + std::min(result.size(), 10u),
                [](const int e) { cout << e << " "; });
  if (result.size() > 10u) {
    cout << "... (" << result.size() - 10u << " more)";
  }
  cout << endl;
}

