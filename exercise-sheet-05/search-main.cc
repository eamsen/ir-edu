// Copyright 2012 Eugen Sawin <esawin@me73.com>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <queue>
#include <limits>
#include "./index.h"
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
// The default n-gram value for n.
static const int kNGramN = 3;

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

// Writes the given record and score to the stream.
void WriteUrlScore(const Index::Record& record, const float score,
                   ostream* stream) {
  *stream << record.url << " (" << score << ")\n";
}

// Writes the given record to the given stream, highlighting the given matches.
// Matches are given as (position, size) pairs.
void WriteRecord(const Index::Record& record, const float score,
                 const vector<pair<size_t, size_t> >& matches,
                 ostream* stream) {
  WriteUrlScore(record, score, stream);
  std::priority_queue<pair<int, int>, vector<pair<int, int> >,
                      std::greater<pair<int, int> > > queue;
  for (auto it = matches.cbegin(), end = matches.cend();
       it != end; ++it) {
    queue.push(*it);
  }
  size_t pos = 0u;
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

// Writes the snippets of given record to the given stream,
// highlighting the given matches.
// Matches are given as (position, size) pairs.
// TODO(esawin): Shorten output, make it digestible.
void WriteRecordSnippets(const Index::Record& record, const float score,
                         const vector<pair<size_t, size_t> >& matches,
                         const size_t offset,
                         ostream* stream) {
  WriteUrlScore(record, score, stream);
  std::priority_queue<pair<int, int>, vector<pair<int, int> >,
                      std::greater<pair<int, int> > > queue;
  for (auto it = matches.cbegin(), end = matches.cend();
       it != end; ++it) {
    queue.push(*it);
  }
  size_t next_end = 0u;
  size_t pos = 0u;
  while (queue.size()) {
    const size_t match_beg = queue.top().first;
    const size_t match_size = queue.top().second;
    queue.pop();
    if (next_end < match_beg - offset) {
      *stream << record.content.substr(pos, next_end - pos) << " ... ";
      pos = next_end;
    }
    if (pos <= match_beg) {
      const size_t beg = std::max(static_cast<int>(pos),
                                  static_cast<int>(match_beg) -
                                  static_cast<int>(offset));
      *stream << record.content.substr(beg, match_beg - beg)
              << kBoldText << record.content.substr(match_beg, match_size)
              << kResetMode;
      next_end = match_beg + match_size + offset;
    }
    pos = match_beg + match_size;
  }
  if (pos < next_end) {
    *stream << record.content.substr(pos, next_end - pos);
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
  vector<string> args(&argv[0], &argv[argc]);
  if (argc != 2 && argc != 3 && argc != 4 && argc != 6) {
    cout << "Usage: search-main <CSV-file> [<num-records>] [<n-gram n>] "
         << "[<BM25-b> <BM25-k>]" << endl;
    return 1;
  }
  const string filename = args[1];
  size_t max_num_records = 3;
  int ngram_n = kNGramN;
  float bm25_b = 0.75f;
  float bm25_k = 1.75f;
  if (argc >= 3) {
    // The second command-line argument sets the max number of records to be
    // viewed.
    std::stringstream(args[2]) >> max_num_records;
    if (max_num_records == 0) {
      max_num_records = std::numeric_limits<size_t>::max();
    }
  }
  if (argc >= 4) {
    // The thirds argument sets the n value for the n-gram index construction.
    std::stringstream(args[3]) >> ngram_n;
    if (ngram_n < 3 || ngram_n > 7) {
      cout << "Choose a value n with 2 < n < 7 for the n-gram construction"
           << endl;
      return 1;
    }
  }
  if (argc == 6) {
    // The forth and fifth argument sets the BM25 parameters.
    std::stringstream(args[4]) >> bm25_b;
    std::stringstream(args[5]) >> bm25_k;
  }
  Index index;
  Profiler::Start("index-construction.prof");
  auto start = Clock();
  Index::AddRecordsFromCsv(ReadFile(filename), &index);
  index.ComputeScores(bm25_b, bm25_k);
  index.BuildNGrams(ngram_n);
  auto end = Clock();
  Profiler::Stop();
  auto diff = end - start;
  cout << "Number of records: " << index.NumRecords()
       << "\nNumber of items: " << index.NumItems()
       << "\nIndex construction time: " << Clock::DiffStr(diff)
       << "\nShow top " << max_num_records << " results"
       << "\nN-gram value: " << ngram_n
       << "\nBM25 parameters: b = " << bm25_b << ", k = " << bm25_k
       << "\nType q to quit\n";

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
        // WriteRecordSnippets(record, prev_score, matches, 20u, &cout);
        WriteUrlScore(record, prev_score, &cout);
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
  return 0;
}
