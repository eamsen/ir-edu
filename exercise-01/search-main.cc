// Copyright 2012 Eugen Sawin <esawin@me73.com>

#include <string>
#include "./inverted-index.h"
#include "../profiler.h"

using std::string;

// Main function.
int main(int argc, char** argv) {
  // Parse command line arguments.
  if (argc != 2) {
    printf("Usage: search-main <name of collection CSV file>\n");
    exit(1);
  }
  const string filename = argv[1];
  Index index;
  Profiler::Start("csv-parse.prof");
  Index::AddRecordsFromCsvFile(filename, &index);
  Profiler::Stop();
  // index.OutputInvertedListLengths();
}
