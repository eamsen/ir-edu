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
#include <queue>

using std::unordered_map;
using std::string;
using std::vector;

const int Index::kInvalidId = -1;
const char* Index::kWhitespace = "\n\r\t ";

size_t Index::kMinKeywordSize = 2;
uint8_t Index::kUtf8RepairReplace = '_';

int Index::RepairUtf8(string* s) {
  const size_t size = s->size();
  const uint8_t* end = reinterpret_cast<uint8_t*>(&(*s)[size]);
  int num_repaired = 0;

  // Returns the pointer to the last valid byte starting at given byte. If the
  // character encoding starting at the given byte is not valid, the given
  // start pointer is returned. Also, it reduces byte sequences, which do not
  // use the minumum sequence to encode a character.
  auto LastValid = [&s, end, &num_repaired](uint8_t* b) -> uint8_t* {
    static const vector<uint8_t> _len_map =
      {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 3, 4};
    uint8_t* c = b;
    // Number of leading 1s encode the sequence length.
    uint8_t seq_len = _len_map[*c >> 4];
    if (seq_len > 1 && c + seq_len - 1 < end) {
      // Byte sequence start with enough bytes left in the string to encode it.
      if (seq_len == 2 && (*c & 30u) == 0) {
        // Wasted byte, move the last bit to the next byte.
        uint8_t* c_prev = c++;
        *c = (*c | ((*c_prev & 1u) << 6)) & 127u;
        *c_prev = kUtf8RepairReplace;
        seq_len = 1;
        num_repaired += 2;
      } else if ((*c << seq_len) == 0 && (*(c + 1) >> (6 - seq_len)) & 1u) {
        // Wasted byte, move sequence start to the next byte.
        *c++ = kUtf8RepairReplace;
        *c = (((seq_len & 6u) + 10u) << 4) | *c;
        seq_len = 1;
        num_repaired += 2;
      }
      while (--seq_len) {
        ++c;
        if (*c < 128 || *c > 191) {
          // Most significant bits are not 10.
          break;
        }
      }
    }
    return seq_len ? b : c;
  };

  uint8_t* c = reinterpret_cast<uint8_t*>(&(*s)[0]);
  while (c < end) {
    if (*c < 128) {
      // ASCII, move on.
      ++c;
      continue;
    }
    uint8_t* last_valid = LastValid(c);
    if (last_valid == c) {
      // Invalid sequence begin.
      *c = kUtf8RepairReplace;
      ++num_repaired;
    }
    assert(last_valid - c < 4);
    c = last_valid + 1;
  }
  return num_repaired;
}

vector<string> Index::Split(const string& content, const string& delims) {
  vector<string> items;
  size_t pos = content.find_first_not_of(delims);
  while (pos != string::npos) {
    size_t end = content.find_first_of(delims, pos);
    if (end == string::npos) {
      // Last item found.
      items.push_back(content.substr(pos));
    } else {
      // Item found.
      items.push_back(content.substr(pos, end - pos));
    }
    pos = content.find_first_not_of(delims, end);
  }
  return items;
}

auto Index::ExtractKeywords(const string& content, const size_t beg,
                            const size_t end) -> vector<PosSize> {
  using std::isalnum;

  // Keyword density approximation.
  static float _density = 1.0f / kMinKeywordSize;

  const size_t content_size = content.size();
  assert(beg < end && end <= content_size);
  vector<PosSize> keywords;
  keywords.reserve(content_size * _density * 1.5f);
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
  _density = (_density * 3.0f +
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

void Index::AddKeywords(const std::string& file_content, Index* index) {
  const size_t content_size = file_content.size();
  size_t pos = 0;
  while (pos < content_size) {
      const size_t key_end = file_content.find('\n', pos);
      assert(key_end != string::npos && "Wrong file format");
      const string keyword = file_content.substr(pos, key_end - pos);
      // Assuming there are no duplicates in the file.
      index->AddKeyword(keyword);
      pos = key_end + 1u;
  }
}

vector<string> Index::NGrams(const string& word, const int ngram_n) {
  assert(ngram_n > 1);
  const size_t word_size = word.size();
  const size_t num_ngrams = word_size - ngram_n + 3;
  if (num_ngrams < 2) {
    return vector<string>();
  }
  vector<string> ngrams;
  ngrams.reserve(num_ngrams);
  ngrams.push_back("#" + word.substr(0, ngram_n - 1));
  for (size_t n = 0; n < num_ngrams - 2; ++n) {
    ngrams.push_back(word.substr(n, ngram_n));
  }
  ngrams.push_back(word.substr(num_ngrams - 2, ngram_n - 1) + "#");
  assert(ngrams.size() == num_ngrams);
  return ngrams;
}

vector<string> Index::NGrams(const vector<string>& words, const int ngram_n) {
  vector<string> ngrams;
  for (auto beg = words.cbegin(), it = beg, end = words.cend();
       it != end; ++it) {
    vector<string> tmp_ngrams = NGrams(*it, ngram_n);
    if (tmp_ngrams.size() == 0) {
      continue;
    }
    auto tmp_beg = tmp_ngrams.begin();
    auto tmp_end = tmp_ngrams.end();
    if (it != beg) {
      // We are not at the beginning, the first n-gram is not valid.
      ++tmp_beg;
    }
    if (it != end - 1) {
      // We are not at the end, the last n-gram is not valid.
      --tmp_end;
    }
    if (tmp_beg < tmp_end) {
      // We still have n-grams left, remember them.
      ngrams.insert(ngrams.end(), tmp_beg, tmp_end);
    }
  }
  return ngrams;
}

int Index::EditDistance(const string& word1, const string& word2) {
  auto Dist = [](int r, int d, int i, bool eq) {
    return std::min(std::min(d, i) + 1, r + !eq);
  };

  const size_t size1 = word1.size();
  const size_t size2 = word2.size();
  if (size1 > size2) {
    return EditDistance(word2, word1);
  }
  vector<int> dists(size1 + 1, 0);
  for (size_t i = 1; i < size1 + 1; ++i) {
    dists[i] = i;
  }
  vector<int> new_dists(size1 + 1, 0);
  for (size_t w2 = 0; w2 < size2; ++w2) {
    new_dists[0] = dists[0] + 1;
    for (size_t w1 = 0; w1 < size1; ++w1) {
      new_dists[w1 + 1] = Dist(dists[w1], dists[w1 + 1], new_dists[w1],
                               word1[w1] == word2[w2]);
    }
    dists.swap(new_dists);
  }
  return dists.back();
}

vector<int> Index::Union(const vector<const vector<int>*>& lists) {
  vector<int> freqs;
  return Union(lists, &freqs);
}

vector<int> Index::Union(const vector<const vector<int>*>& lists,
                         vector<int>* freqs) {
  using std::make_pair;
  typedef std::priority_queue<std::pair<int, int>, vector<std::pair<int, int> >,
                              std::greater<std::pair<int, int> > > Queue;

  assert(freqs);
  const size_t num_lists = lists.size();
  vector<int> indices(num_lists, 0);
  vector<size_t> list_sizes(num_lists, 0);
  Queue queue;
  size_t total_size = 0u;
  for (size_t l = 0u; l < num_lists; ++l) {
    const vector<int>& list = *lists[l];
    list_sizes[l] = list.size();
    if (list_sizes[l]) {
      // Non-empty list.
      queue.push(make_pair(list[0], l));
      total_size += list_sizes[l];
    }
  }

  vector<int> results;
  results.reserve(total_size);
  freqs->clear();
  freqs->reserve(total_size);
  while (queue.size()) {
    const int id = queue.top().first;
    const int list = queue.top().second;
    queue.pop();
    if (results.empty() || results.back() != id) {
      // New id.
      results.push_back(id);
      freqs->push_back(0);
    }
    if (indices[list] + 1u < list_sizes[list]) {
      // Increment the list index for active list and push new id to the queue.
      queue.push(make_pair((*lists[list])[++indices[list]], list));
    }
    // Increase the last id's frequency.
    ++freqs->back();
  }
  return results;
}

Index::Index()
    : num_items_(0u),
      total_size_(0u),
      ngram_n_(0),
      last_ed_avg_duration_(0) {}

vector<string> Index::ApproximateMatches(const std::string& query,
                                         const int max_ed) const {
  assert(ngram_n_ > 1);
  // Generate the n-grams for the given query.
  vector<string> query_parts = Split(query, "*");
  vector<string> ngrams = NGrams(query_parts, ngram_n_);
  // Assemble the inverted lists for the n-grams.
  vector<const vector<int>*> lists;
  lists.reserve(ngrams.size());
  for (const string& ngram: ngrams) {
    const vector<int>& keyword_ids = NGramItems(ngram);
    lists.push_back(&keyword_ids);
  }
  // Merge the lists.
  vector<int> keyword_freqs;
  vector<int> keyword_ids = Union(lists, &keyword_freqs);
  assert(keyword_ids.size() == keyword_freqs.size());
  // Assemble and filter the resulting keyword strings.
  vector<string> keywords;
  keywords.reserve(keyword_ids.size());
  const int query_size = query.size();
  const int max_ed_n = max_ed * ngram_n_;
  size_t num_ed_calls = 0u;
  const Clock beg;
  for (size_t i = 0, num_keywords = keyword_ids.size(); i < num_keywords; ++i) {
    const string& keyword = KeywordById(keyword_ids[i]);
    // TODO(esawin): How to handle queries with wildcards?
    if (keyword_freqs[i] >= std::max(static_cast<int>(keyword.size()),
                                     query_size) - max_ed_n) {
      ++num_ed_calls;
      if (EditDistance(keyword, query) <= max_ed) {
        keywords.push_back(keyword);
      }
    }
  }
  last_ed_avg_duration_ = num_ed_calls ? (Clock() - beg) / num_ed_calls : 0;
  return keywords;
}

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
  static const vector<Item> _kEmptyList;
  const int id = KeywordId(keyword);
  if (id == kInvalidId) {
    return _kEmptyList;
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

const vector<int>& Index::NGramItems(const string& ngram) const {
  static const vector<int> _empty;

  auto it = ngram_index_.find(ngram);
  if (it == ngram_index_.end()) {
    return _empty;
  }
  return it->second;
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

size_t Index::NumKeywords() const {
  return keywords_.size();
}

Clock::Diff Index::LastEdAvgDuration() const {
  return last_ed_avg_duration_;
}
