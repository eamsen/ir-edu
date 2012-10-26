// Copyright 2012 Eugen Sawin <esawin@me73.com>
#include "./inverted-index.h"
#include <unordered_map>
#include <cassert>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

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

  // Keyword density approximation.
  static float density = 1.0f / kMinKeywordSize;

  const size_t content_size = content.size();
  assert(beg < end && end <= content_size);
  vector<PosSize> keywords;
  keywords.reserve(content_size * density * 1.5f);
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
      // Valid keyword need at least one alphabetic character and a certain
      // minimum size.
      if (valid && keyword_size >= kMinKeywordSize) {
        keywords.push_back({key_beg, keyword_size});
      }
    }
    ++pos;
  }
  // Update density approximation.
  density = (density * 3.0f +
             keywords.size() / static_cast<float>(content_size))
            * 0.25f;
  return keywords;
}

void Index::AddRecordsFromCsv(const string& file_content,
                              Index* inverted_index) {
  const size_t content_size = file_content.size();
  size_t pos = 0;
  while (pos < content_size) {
    // Skip to second column after first tab.
    const size_t content_beg = file_content.find('\t', pos);
    assert(content_beg != string::npos && "CSV file has wrong format");
    const size_t content_end = file_content.find('\n', content_beg);
    assert(content_end != string::npos && "CSV file has wrong format");
    assert(content_end > content_beg);
    const string url = file_content.substr(pos, content_beg - pos);
    const string content = file_content.substr(content_beg + 1,
                                               content_end - content_beg);
    const int record_id = inverted_index->AddRecord(url, content);
    vector<PosSize> keywords = ExtractKeywords(content, 0, content.size());
    // Add each keyword from the content to the index.
    for (auto keyIt = keywords.cbegin(), endIt = keywords.cend();
         keyIt != endIt; ++keyIt) {
      const string keyword = content.substr(keyIt->pos, keyIt->size);
      inverted_index->AddItem(keyword, record_id, keyIt->pos);
    }
    pos = content_end + 1;
  }
}

void Index::AddRecordsFromCsvFile(const string& filename,
                                  Index* inverted_index) {
  using std::getline;

  ifstream file(filename.c_str());
  string line;
  getline(file, line);
  while (!file.eof()) {
    // Skip to second column after first tab.
    const size_t pos = line.find('\t');
    assert(pos != string::npos && "CSV file has wrong format");
    const string url = line.substr(pos);
    const string content = line.substr(pos + 1, line.size() - pos);
    const int record_id = inverted_index->AddRecord(url, content);
    vector<PosSize> keywords = ExtractKeywords(content, 0, content.size());
    // Add each keyword from the content to the index.
    for (auto keyIt = keywords.cbegin(), endIt = keywords.cend();
         keyIt != endIt; ++keyIt) {
      const string keyword = content.substr(keyIt->pos, keyIt->size);
      inverted_index->AddItem(keyword, record_id, keyIt->pos);
    }
    getline(file, line);
  }
}

Index::Index()
    : num_items_(0u) {}

const vector<Index::Item>& Index::Items(const string& keyword) const {
  static vector<Item> kEmptyList;

  auto it = index_.find(keyword);
  if (it == index_.end()) {
    return kEmptyList;
  }
  return it->second;
}

int Index::AddRecord(const string& url, const string& content) {
  // TODO(esawin): Check for duplicates.
  records_.push_back({url, content});
  return records_.size() - 1;
}

int Index::AddItem(const string& keyword, const int record_id,
                   const size_t pos) {
  // TODO(esawin): Check for duplicates and sort if required.
  index_[keyword].push_back({record_id, pos});
  return ++num_items_;
}

void Index::ReserveRecords(const size_t num) {
  records_.reserve(num);
}

size_t Index::NumRecords() const {
  return records_.size();
}

size_t Index::NumItems() const {
  return num_items_;
}

void Index::OutputInvertedListLengths() const {
  for (auto it = index_.cbegin(), end = index_.cend();
       it != end; ++it) {
    const string& word = it->first;
    const size_t size = it->second.size();
    std::cout << word << "\t" << size << std::endl;
  }
}
