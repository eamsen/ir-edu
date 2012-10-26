// Copyright 2012, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Author: Hannah Bast <bast@informatik.uni-freiburg.de>.

#include <string>
#include "./inverted-index.h"

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
  Index::AddRecordsFromCsvFile(filename, &index);
  // index.OutputInvertedListLengths();
}
