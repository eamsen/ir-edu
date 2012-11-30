// Copyright 2012 Eugen Sawin <esawin@me73.com>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <queue>
#include <limits>
#include <functional>
#include <cmath>
#include "./index.h"
#include "./profiler.h"
#include "./clock.h"

using std::vector;
using std::string;

// 0: all off, 1: bold, 4: underscore, 5: blinking, 7: reversed, 8: concealed
// 3x: text, 4x: background
// 0: black, 1: red, 2: green, 3: yellow, 4: blue, 5: magenta, 6: cyan, 7: white
static const char* kResetMode = "\033[0m";
static const char* kBoldText = "\033[1m";
// static const char* kUnderscoreText = "\033[4m";
// The default n-gram value for n.
static const int kNGramN = 3;

// Returns the file size of given file. Returns 0, if the file is not found.
size_t FileSize(const string& path) {
  using std::ifstream;

  ifstream stream(path.c_str());
  size_t size = 0;
  if (stream.good()) {
    stream.seekg(0, std::ios::end);
    size = stream.tellg();
    stream.seekg(0, std::ios::beg);
  }
  return size;
}

// Reads the whole file from given path and returns the content.
string ReadFile(const string& path) {
  using std::ifstream;

  const size_t file_size = FileSize(path);
  string content;
  content.resize(file_size);
  ifstream stream(path.c_str());
  stream.read(&content[0], file_size);
  return content;
}

// Returns the average duration in microseconds for the execution of given
// function over given number of iterations.
template<typename Func>
Clock::Diff AvgDuration(Func func, const size_t num_iter) {
  const auto beg = Clock();
  for (size_t i = 0; i < num_iter; ++i) {
    func();
  }
  return (Clock() - beg) / num_iter;
}

// Returns the duration in microseconds for the execution of given function.
template<typename Func>
Clock::Diff Duration(Func func) {
  const auto beg = Clock();
  func();
  return Clock() - beg;
}

// Main function.
int main(int argc, char** argv) {
  using std::cout;
  using std::cin;
  using std::getline;
  using std::endl;
  using std::flush;

  // Parse command line arguments.
  vector<string> args(&argv[0], &argv[argc]);
  if (argc != 3 && argc != 4) {
    cout << "Usage: exercise05-main <keyword-file> <queries-file> [<n-gram n>] "
         << endl;
    return 1;
  }
  const string keywords_filename = args[1];
  const string queries_filename = args[2];
  int ngram_n = kNGramN;
  if (argc == 4) {
    // The third argument sets the n value for the n-gram index construction.
    std::stringstream(args[3]) >> ngram_n;
    if (ngram_n < 3 || ngram_n > 7) {
      cout << "Choose a value n with 2 < n < 7 for the n-gram construction"
           << endl;
      return 1;
    }
  }
  Index index;
  Profiler::Start("index-construction.prof");
  auto index_time = Duration(std::bind(Index::AddKeywords,
                                       ReadFile(keywords_filename),
                                       &index));
  index_time += Duration(std::bind(&Index::BuildNGrams, &index, ngram_n));
  Profiler::Stop();
  cout << "Number of keywords: " << index.NumKeywords()
       << "\nN-gram value: " << ngram_n
       << "\nIndex construction time: "
       << kBoldText << index_time << kResetMode
       << endl;

  // Run the experiement on the queries file.
  Clock::Diff query_time = 0;
  Clock::Diff ed_time = 0;
  size_t num_matches = 0u;
  size_t num_queries = 0u;
  string query_content = ReadFile(queries_filename);
  const size_t content_size = query_content.size();
  size_t pos = 0;
  while (pos < content_size) {
      const size_t query_end = query_content.find('\n', pos);
      assert(query_end != string::npos && "Wrong file format");
      const string query = query_content.substr(pos, query_end - pos);
      pos = query_end + 1u;
      query_time += Duration([&num_matches, &index, &query]() {
        num_matches += index.ApproximateMatches(query, std::ceil(query.size() /
                                                                 5.0f)).size();
      });
      ed_time += index.LastEdAvgDuration();
      ++num_queries;
  }
  cout << "Avg number of matches: "
       << kBoldText << num_matches / num_queries << kResetMode
       << "\nAvg query time: "
       << kBoldText << (query_time / num_queries) << kResetMode
       << "\nAvg edit distance time: "
       << kBoldText << (ed_time / num_queries) << kResetMode
       << endl;
  return 0;
}
