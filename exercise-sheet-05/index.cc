// Copyright 2012 Eugen Sawin <esawin@me73.com>
#include "./index.h"
#include <unordered_map>
#include <cassert>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>

using std::unordered_map;
using std::string;
using std::vector;

const size_t Index::kMinKeywordSize = 2;
const int Index::kInvalidId = -1;

auto Index::ExtractKeywords(const string& content, const size_t beg,
                            const size_t end) -> vector<PosSize> {
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

void Index::AddRecordsFromCsv(const string& file_content, Index* index) {
  const size_t content_size = file_content.size();
  string prev_url;
  int record_id = kInvalidId;
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
    size_t offset = 0u;
    // Assuming the records are continuous within the list.
    // TODO(esawin): assert that!
    if (url != prev_url) {
      // New record found in file contents.
      assert(url.size());
      record_id = index->AddRecord(url, content);
      prev_url = url;
    } else {
      // Known record, add the content.
      offset = index->ExtendRecord(record_id, content);
    }
    // Extract the keyword positions.
    const vector<PosSize> keywords = ExtractKeywords(content, 0,
                                                     content.size());
    // Add each keyword from the content to the index.
    for (auto it = keywords.cbegin(), end = keywords.cend();
         it != end; ++it) {
      // Extract the keyword string and add it to the index.
      const string keyword = content.substr(it->pos, it->size);
      int keyword_id = index->KeywordId(keyword);
      if (keyword_id == kInvalidId) {
        // New keyword.
        keyword_id = index->AddKeyword(keyword);
      }
      index->AddItem(keyword_id, record_id, it->pos + offset);
    }
    pos = content_end + 1;
  }
  // TODO(esawin): Should we call CalculateScores? This would degrade this
  // function to a constructor.
}

vector<string> Index::NGrams(const string& word, const int ngram_n) {
  assert(ngram_n > 1);
  const size_t word_size = word.size();
  const int num_ngrams = word_size - ngram_n + 3;
  assert(num_ngrams > 1);
  assert(word_size > 1);
  vector<string> ngrams(num_ngrams);
  ngrams[0] = "#" + word.substr(0, ngram_n - 1);
  for (int n = 1; n < num_ngrams - ngram_n; ++n) {
    ngrams[n] = word.substr(n - 1, ngram_n);
  }
  ngrams.back() = word.substr(word_size - ngram_n + 1, ngram_n - 1) + "#";
  return ngrams;
}

int Index::EditDistance(const string& word1, const string& word2) {
  auto Dist = [](int r, int d, int i, bool eq) {
    return std::min(std::min(d, i) + 1, r + !eq);
  };

  const int size1 = word1.size();
  const int size2 = word2.size();
  if (size1 > size2) {
    return EditDistance(word2, word1);
  }
  vector<int> dists(size1 + 1, 0);
  for (int i = 1; i < size1 + 1; ++i) {
    dists[i] = i;
  }
  vector<int> new_dists(size1 + 1, 0);
  for (int w2 = 0; w2 < size2; ++w2) {
    new_dists[0] = dists[0] + 1;
    for (int d1 = 1; d1 < size1 + 1; ++d1) {
      new_dists[d1] = Dist(dists[d1 - 1], dists[d1], new_dists[d1 - 1],
                           word1[d1 - 1] == word2[w2]);
    }
    std::swap(dists, new_dists);
  }
  return dists.back();
}

Index::Index()
    : num_items_(0u),
      total_size_(0u) {}

void Index::ComputeScores(const float b, const float k) {
  const float num_records = NumRecords();
  const float inv_avg_record_size = num_records / TotalSize();
  for (auto it = keywords_.begin(), end = keywords_.end(); it != end; ++it) {
    vector<Item>& items = it->items;
    const float record_freq = items.size();
    assert(record_freq >= 1.0f);
    const float inv_record_freq = std::log2(num_records / record_freq);
    for (auto it2 = items.begin(), end2 = items.end(); it2 != end2; ++it2) {
      Item& item = *it2;
      const float record_size  = RecordById(item.record_id).content.size();
      item.score = item.score * (k + 1.0f) /
                   (k * (1.0f - b + b * record_size * inv_avg_record_size) +
                    item.score) * inv_record_freq;
    }
  }
}

void Index::BuildNGrams(const int ngram_n) {
  assert(ngram_n > 1);
  ngram_n_ = ngram_n;

  const size_t num_keywords = keywords_.size();
  for (size_t keyword_id = 0; keyword_id < num_keywords; ++keyword_id) {
    const Keyword& keyword = KeywordById(keyword_id);
    vector<string> ngrams = NGrams(keyword, ngram_n_);
    for (const string& ngram: ngrams) {
      AddNGram(keyword_id, ngram);
    }
  }
}

const Index::Record& Index::RecordById(const int record_id) const {
  assert(record_id >= 0 && record_id < static_cast<int>(records_.size()));
  return records_[record_id];
}

Index::Record& Index::recordById(const int record_id) {
  assert(record_id >= 0 && record_id < static_cast<int>(records_.size()));
  return records_[record_id];
}

auto Index::Items(const string& keyword) const -> const vector<Item>& {
  static vector<Item> kEmptyList;
  const int id = KeywordId(keyword);
  if (id == kInvalidId) {
    return kEmptyList;
  }
  return KeywordById(id).items;
}

int Index::KeywordId(const string& keyword) const {
  string low = keyword;
  std::transform(keyword.cbegin(), keyword.cend(), low.begin(), ::tolower);
  auto const it = keyword_index_.find(low);
  if (it == keyword_index_.end()) {
    return kInvalidId;
  }
  return it->second;
}

int Index::AddRecord(const string& url, const string& content) {
  // TODO(esawin): Check for duplicates.
  records_.push_back({url, content});
  total_size_ += content.size();
  return records_.size() - 1;
}

size_t Index::ExtendRecord(const int record_id, const string& content) {
  Record& record = recordById(record_id);
  const size_t size = record.content.size();
  record.content += content;
  total_size_ += content.size();
  return size;
}

int Index::AddItem(const int keyword_id, const int record_id,
                   const size_t pos) {
  Keyword& keyword = keywordById(keyword_id);
  if (keyword.items.size() && keyword.items.back().record_id == record_id) {
    // Add another keyword position within known record and increase term
    // frequency.
    keyword.items.back().positions.push_back(pos);
    keyword.items.back().score += 1.0f;
  } else {
    // Keyword occurs in a new record.
    keyword.items.push_back(Item(record_id, {pos}, keyword.name.size(), 1.0f));
  }
  return ++num_items_;
}

void Index::AddNGram(const int keyword_id, const string& ngram) {
  auto it = ngram_index_.find(ngram);
  if (it == ngram_index_.end()) {
    // New n-gram.
    ngram_index_.insert(std::make_pair(ngram, vector<int>({keyword_id})));
  } else {
    // Add keyword to known n-gram.
    it->second.push_back(keyword_id);
  }
}

int Index::AddKeyword(const string& keyword) {
  string low = keyword;
  std::transform(keyword.cbegin(), keyword.cend(), low.begin(), ::tolower);
  int id = keywords_.size();
  keywords_.push_back(Keyword(low));
  keyword_index_.insert(std::make_pair(low, id));
  return id;
}

const Index::Keyword& Index::KeywordById(const int id) const {
  assert(id >= 0 && id < static_cast<int>(keywords_.size()));
  return keywords_[id];
}

Index::Keyword& Index::keywordById(const int id) {
  assert(id >= 0 && id < static_cast<int>(keywords_.size()));
  return keywords_[id];
}

void Index::ReserveRecords(const size_t num) {
  records_.reserve(num);
}

size_t Index::TotalSize() const {
  return total_size_;
}

size_t Index::NumRecords() const {
  return records_.size();
}

size_t Index::NumItems() const {
  return num_items_;
}
