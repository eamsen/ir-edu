// Copyright 2012 Eugen Sawin <esawin@me73.com>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <queue>
#include "./inverted-index.h"
#include "./query-processor.h"
#include "./profiler.h"
#include "./clock.h"

using std::vector;
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

// Returns the file size of given file. Returns 0, if the file is not found.
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

// Reads the whole file from given path and returns the content.
string ReadFile(const string& path) {
  const size_t file_size = FileSize(path);
  string content;
  content.resize(file_size);
  ifstream stream(path.c_str());
  stream.read(&content[0], file_size);
  return content;
}

// Writes the given record to the given stream, highlighting the given matches.
// Matches are given as (position, size) pairs.
void WriteRecord(const Index::Record& record,
                 const vector<pair<size_t, size_t> >& matches,
                 ostream* stream) {
  *stream << "\n" << record.url;
  // Do not output content for now.
  return;
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
  using std::cout;
  using std::cin;
  using std::getline;
  using std::endl;
  using std::flush;

  // Parse command line arguments.
  if (argc != 2 && argc != 3) {
    cout << "Usage: search-main <CSV-file> [<num-records>]" << endl;
    return 1;
  }
  const string filename = argv[1];
  size_t max_num_records = 3;
  if (argc == 3) {
    // The second command-line argument sets the max number of records to be
    // viewed.
    std::stringstream ss;
    ss << argv[2];
    ss >> max_num_records;
  }
  Index index;
  Profiler::Start("index-construction.prof");
  auto start = Clock();
  // Construct index from CSV file line by line.
  // Index::AddRecordsFromCsvFile(filename, &index);
  // Alternative index construction by ready whole CSV file at once.
  Index::AddRecordsFromCsv(ReadFile(filename), &index);
  index.ComputeScores(1.75f, 0.75f);
  auto end = Clock();
  Profiler::Stop();
  auto diff = end - start;
  // Output word frequencies, only required for exercise.
  // index.OutputInvertedListLengths();
  cout << "Number of records: " << index.NumRecords();
  cout << "\nNumber of items: " << index.NumItems();
  cout << "\nIndex construction duration: " << Clock::DiffStr(diff);
  cout << "\nType q to quit\n";

  QueryProcessor proc(index);
  while (true) {
    string query;
    // Get user query.
    cout << "\nSearch: " << kUnderscoreText << flush;
    getline(cin, query);
    cout << kResetMode;

    if (query == "q") {
      // User has chosen to quit.
      break;
    }

    // Process query, get matching records.
    vector<Index::Item> results = proc.Answer(query, max_num_records);
    const size_t records_found = proc.LastRecordsFound();
    auto const duration = proc.LastDuration();
    if (results.size() == 0) {
      // No records found.
      cout << kBoldText << "\nNothing found in "
           << Clock::DiffStr(duration) << "\n" << kResetMode;
    } else {
      cout << "Found " << records_found << " record"
           << (records_found > 1 ? "s" : "") << " in "
           << Clock::DiffStr(duration) << "\n";
    }

    size_t num_records = 0;
    vector<pair<size_t, size_t> > matches;
    int prev_record_id = Index::kInvalidId;
    float prev_score = 0;
    const size_t num_show_records = std::min(records_found, max_num_records);
    // Iterate over results to output all matching records.
    // Result items are sorted by record ids, with one item for each keyword
    // occurrence.
    while (num_records < num_show_records) {
      if ((results.size() &&
           results.back().record_id != prev_record_id &&
           matches.size()) ||
          results.empty()) {
        // Found new/last record id, so we output the matches for the previous
        // record id.
        const Index::Record& record = index.RecordById(prev_record_id);
        WriteRecord(record, matches, &cout);
        cout << ": " << prev_score << "\n";
        ++num_records;
        matches.clear();
      }
      if (results.size()) {
        // Remember the keyword occurrence until all occurrences are
        // collected for the current record id.
        const Index::Item& item = results.back();
        for (auto it = item.positions.cbegin(), end = item.positions.cend();
             it != end; ++it) {
          matches.push_back(make_pair(*it, item.size));
        }
        prev_record_id = item.record_id;
        prev_score = item.score;
        results.pop_back();
      }
    }
  }
  cout << "Bye!" << endl;
}
