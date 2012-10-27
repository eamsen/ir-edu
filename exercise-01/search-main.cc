// Copyright 2012 Eugen Sawin <esawin@me73.com>

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <queue>
#include "./inverted-index.h"
#include "./query-processor.h"
#include "../profiler.h"
#include "../clock.h"

using std::vector;
using std::cout;
using std::cin;
using std::getline;
using std::endl;
using std::flush;
using std::string;
using std::ifstream;
using std::ostream;
using std::pair;
using std::make_pair;

// 0: all off, 1: bold, 4: underscore, 5: blinking, 7: reversed, 8: concealed
// 3x: text, 4x: background
// 0: black, 1: red, 2: green, 3: yellow, 4: blue, 5: magenta, 6: cyan, 7: white
static const char* kResetMode = "\033[0m";
static const char* kBoldText = "\033[1m";
static const char* kUnderscoreText = "\033[4m";
// static const char* kBlinkText = "\033[5m";
// static const char* kReverseText = "\033[7m";
// static const char* kConcealedText = "\033[8m";
// static const char* kYellowText = "\033[0;33m";
// static const char* kMagentaText = "\033[0;35m";

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

void WriteRecord(const Index::Record& record,
                 const vector<pair<size_t, size_t> >& matches,
                 ostream* stream) {
  *stream << "\n" << record.url << "\n";
  std::priority_queue<pair<int, int>, vector<pair<int, int> >,
                      std::greater<pair<int, int> > > queue;
  for (auto it = matches.cbegin(), end = matches.cend();
       it != end; ++it) {
    queue.push(*it);
  }
  size_t pos = 0;
  while (queue.size()) {
    const size_t match_beg = queue.top().first;
    const size_t match_size = queue.top().second;
    queue.pop();
    if (pos <= match_beg) {
      *stream << record.content.substr(pos, match_beg - pos)
              << kBoldText << record.content.substr(match_beg, match_size)
              << kResetMode;
    }
    pos = match_beg + match_size;
  }
  if (pos < record.content.size()) {
    *stream << record.content.substr(pos, record.content.size() - pos);
  }
}

// Main function.
int main(int argc, char** argv) {
  // Parse command line arguments.
  if (argc != 2 && argc != 3) {
    cout << "Usage: search-main <CSV-file> [<num-records>]" << endl;
    return 1;
  }
  const string filename = argv[1];
  int max_num_records = 3;
  if (argc == 3) {
    std::stringstream ss;
    ss << argv[2];
    ss >> max_num_records;
  }
  Index index;
  Profiler::Start("index-construction.prof");
  auto start = Clock();
  // Index::AddRecordsFromCsvFile(filename, &index);
  Index::AddRecordsFromCsv(ReadFile(filename), &index);
  auto end = Clock();
  Profiler::Stop();
  auto diff = end - start;
  // index.OutputInvertedListLengths();
  cout << "Number of records: " << index.NumRecords();
  cout << "\nNumber of items: " << index.NumItems();
  cout << "\nIndex construction duration: " << Clock::DiffStr(diff);
  cout << "\nType q to quit\n";

  while (true) {
    string query;
    cout << "\nSearch: " << kUnderscoreText << flush;
    getline(cin, query);
    cout << kResetMode;

    if (query == "q") {
      break;
    }

    QueryProcessor proc(index);
    vector<Index::Item> results = proc.Answer(query, max_num_records);
    if (results.size() == 0) {
      cout << kBoldText << "\nNothing found\n" << kResetMode;
    }

    int num_records = 0;
    vector<pair<size_t, size_t> > matches;
    int prev_record_id = Index::kInvalidId;
    while (results.size() && num_records < max_num_records - 1) {
      const Index::Item& item = results.back();
      if (item.record_id != prev_record_id && matches.size()) {
        const Index::Record& record = index.RecordById(prev_record_id);
        WriteRecord(record, matches, &cout);
        ++num_records;
        matches.clear();
      }
      matches.push_back(make_pair(item.pos, item.size));
      prev_record_id = item.record_id;
      results.pop_back();
    }
    if (matches.size()) {
      const Index::Record& record = index.RecordById(prev_record_id);
      WriteRecord(record, matches, &cout);
    }
  }
  cout << "Bye!" << endl;
}
