// Copyright 2012 Eugen Sawin <esawin@me73.com>

#include "./inverted-index.h"
#include <unordered_map>
#include <cassert>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

using std::ifstream;
using std::unordered_map;
using std::string;
using std::vector;

const size_t Index::kMinKeywordSize = 2;
const int Index::kInvalidId = -1;

vector<Index::PosSize> Index::ExtractKeywords(const string& content,
                                              const size_t beg,
                                              const size_t end) {
  using std::isalnum;
  assert(beg < end && end <= content.size());

  vector<PosSize> keywords;
  size_t pos = beg;
  while (pos < end) {
    // Found beginning of the next keyword (any alphanumeric character).
    if (isalnum(content[pos])) {
      bool valid = isalpha(content[pos]);
      const size_t key_beg = pos;
      ++pos;
      while (pos < end && isalnum(content[pos])) {
        valid = valid || isalpha(content[pos]);
        ++pos;
      }
      // Found end of keyword.
      const size_t keyword_size = pos - key_beg;
      // Valid keyword needs at least one alphabetic character and a certain
      // size.
      if (valid && keyword_size >= kMinKeywordSize) {
        keywords.push_back({key_beg, keyword_size});
      }
    }
    ++pos;
  }
  return keywords;
}

void Index::AddRecordsFromCsvFile(const string& file_name,
                                          Index* inverted_index) {
  using std::getline;

  ifstream file(file_name.c_str());
  string line;
  getline(file, line);
  while (!file.eof()) {
    // Skip to second column after first tab.
    const size_t pos = line.find('\t');
    assert(pos != string::npos && "CSV file has wrong format");
    const string url = line.substr(pos);
    const string content = line.substr(pos + 1, line.size() - pos);
    const int record_id = inverted_index->AddRecord(url, content);
    vector<PosSize> keywords = ExtractKeywords(line, pos + 1, line.size());
    // Add each keyword from the content to the index.
    for (auto keyIt = keywords.cbegin(), endIt = keywords.cend();
         keyIt != endIt; ++keyIt) {
      const string keyword = line.substr(keyIt->pos, keyIt->size);
      inverted_index->AddItem(keyword, record_id);
    }
    getline(file, line);
  }
}

Index::Index()
    : num_items_(0u) {}

int Index::AddRecord(const string& url, const string& content) {
  // TODO(esawin): Check for duplicates.
  records_.push_back({url, content});
  return records_.size() - 1;
}

int Index::AddItem(const string& keyword, const int record_id) {
  // TODO(esawin): Check for duplicates.
  index_[keyword].push_back(record_id);
  return ++num_items_;
}

void Index::OutputInvertedListLengths() const {
  using std::cout;
  using std::endl;

  for (auto it = index_.cbegin(), end = index_.cend();
       it != end; ++it) {
    const string& word = it->first;
    const size_t size = it->second.size();
    cout << word << "\t" << size << endl;
  }
}
