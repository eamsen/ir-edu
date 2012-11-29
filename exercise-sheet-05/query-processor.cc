// Copyright 2012 Eugen Sawin <esawin@me73.com>
#include "./query-processor.h"
#include <cassert>
#include <queue>
#include <algorithm>

using std::string;
using std::vector;

const char* kWhitespace = "\n\r\t ";

vector<string> Split(const string& content, const string& delims) {
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

QueryProcessor::QueryProcessor(const Index& index)
    : index_(index),
      last_num_records_(0u),
      last_duration_(0u) {}

vector<Index::Item> QueryProcessor::Answer(const string& query,
                                           const size_t max_num_records) const {
  auto const beg = Clock();
  vector<const vector<Index::Item>*> lists;
  vector<string> keywords = Split(query, kWhitespace);
  for (auto it = keywords.cbegin(), end = keywords.cend();
       it != end; ++it) {
    const string& keyword = *it;
    const vector<Index::Item>& items = index_.Items(keyword);
    if (items.size()) {
      // Consider this keyword's items, ignore unknown keywords.
      lists.push_back(&items);
    } else {
      // Add to ignored keywords list.
    }
  }
  // Boolean intersection.
  vector<Index::Item> results = Intersect(lists);
  results = Rank(results, max_num_records, lists.size());
  last_duration_ = Clock() - beg;
  return results;
}

vector<Index::Item> QueryProcessor::Rank(const vector<Index::Item>& items,
                                         const size_t max_num_records,
                                         const size_t num_keywords) const {
  typedef std::pair<float, size_t> ScoreIndexPair;

  const size_t num_items = items.size();
  vector<ScoreIndexPair> pairs;
  pairs.reserve(num_items / std::max(1u, num_keywords));
  int prev_record_id = Index::kInvalidId;
  for (size_t i = 0; i < num_items; ++i) {
    const Index::Item& item = items[i];
    if (item.record_id != prev_record_id) {
      // New record.
      pairs.push_back({0.0f, i});
    }
    pairs.back().first += item.score;
    prev_record_id = item.record_id;
  }
  // Sort for the top records.
  size_t pair_index = std::min(max_num_records, pairs.size());
  std::partial_sort(pairs.begin(), pairs.begin() + pair_index, pairs.end(),
                    std::greater<ScoreIndexPair>());
  // Construct the result in reversed order.
  vector<Index::Item> result;
  result.reserve(pair_index * num_keywords);
  while (pair_index--) {
    const float score = pairs[pair_index].first;
    size_t item_index = pairs[pair_index].second;
    const int record_id = items[item_index].record_id;
    while (items[item_index].record_id == record_id) {
      result.push_back(items[item_index++]);
      result.back().score = score;
    }
  }
  return result;
}

vector<Index::Item> QueryProcessor::Intersect(
    const vector<const vector<Index::Item>*>& lists) const {
  using std::make_pair;
  typedef std::priority_queue<std::pair<int, int>, vector<std::pair<int, int> >,
                              std::greater<std::pair<int, int> > > Queue;

  last_num_records_ = 0u;
  const size_t num_lists = lists.size();
  vector<int> indices(num_lists, 0);
  Queue queue;
  size_t max_list_size = 0;
  for (size_t l = 0; l < num_lists; ++l) {
    const vector<Index::Item>& list = (*lists[l]);
    queue.push(make_pair(list[indices[l]].record_id, l));
    std::max(max_list_size, list.size());
  }

  vector<Index::Item> results;
  // TODO(esawin): Shouldn't this be the size of the smallest list?
  results.reserve(num_lists * max_list_size);
  while (queue.size()) {
    const int record_id = queue.top().first;
    const int list = queue.top().second;
    queue.pop();
    if (results.size() && results.back().record_id == record_id) {
      // Current item is another match for an approved intersection.
      results.push_back((*lists[list])[indices[list]]);
    } else {
      // Test whether the item itersects.
      size_t l = 0;
      while (l < num_lists && (*lists[l])[indices[l]].record_id == record_id) {
        ++l;
      }
      if (l == num_lists) {
        // Intersection found; add the current item to the results.
        ++last_num_records_;
        results.push_back((*lists[list])[indices[list]]);
      }
    }
    if (indices[list] + 1u < lists[list]->size()) {
      // Increment the list index for active list.
      queue.push(make_pair((*lists[list])[++indices[list]].record_id, list));
    }
  }
  return results;
}

size_t QueryProcessor::LastRecordsFound() const {
  return last_num_records_;
}

Clock::Diff QueryProcessor::LastDuration() const {
  return last_duration_;
}
