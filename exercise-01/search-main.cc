// Copyright 2012 Eugen Sawin <esawin@me73.com>

#include <iostream>
#include <string>
#include <fstream>
#include "./inverted-index.h"
#include "../profiler.h"
#include "../clock.h"

using std::cout;
using std::cin;
using std::getline;
using std::endl;
using std::flush;
using std::string;
using std::ifstream;

size_t FileSize(const string& path) {
  ifstream stream(path.c_str());
  size_t size = 0;
  if (stream.good()) {
    stream.seekg(0, std::ios::end);
    size = stream.tellg();
    stream.seekg(0, std::ios::beg);
  }
  return size;
}

string ReadFile(const string& path) {
  const size_t file_size = FileSize(path);
  string content;
  content.resize(file_size);
  ifstream stream(path.c_str());
  stream.read(&content[0], file_size);
  return content;
}

// Main function.
int main(int argc, char** argv) {
  // Parse command line arguments.
  if (argc != 2) {
    printf("Usage: search-main <name of collection CSV file>\n");
    exit(1);
  }
  const string filename = argv[1];
  Index index;
  Profiler::Start("index-construction.prof");
  auto start = Clock();
  // Index::AddRecordsFromCsvFile(filename, &index);
  Index::AddRecordsFromCsv(ReadFile(filename), &index);
  auto end = Clock();
  Profiler::Stop();
  auto diff = end - start;
  cout << "Number of records: " << index.NumRecords();
  cout << "\nNumber of items: " << index.NumItems();
  cout << "\nIndex construction duration: " << Clock::DiffStr(diff);
  // index.OutputInvertedListLengths();
  cout << "\n\nSearch : " << flush;
  string query;
  getline(cin, query);
  cout << "\n" << query << endl;
}
