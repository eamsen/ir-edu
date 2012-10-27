// Copyright 2012 Eugen Sawin <esawin@me73.com>
#include "./query-processor.h"
#include <queue>

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
      last_num_records_(0),
      last_duration_(0u) {}

vector<Index::Item> QueryProcessor::Answer(const string& query,
                                           const int max_num_records) const {
  auto const beg = Clock();
  vector<const vector<Index::Item>*> lists;
  vector<string> keywords = Split(query, kWhitespace);
  for (auto it = keywords.cbegin(), end = keywords.cend();
       it != end; ++it) {
    const string& keyword = *it;
    const vector<Index::Item>& items = index_.Items(keyword);
    if (items.size()) {
      lists.push_back(&items);
    }
  }
  vector<Index::Item> results = Intersect(lists, max_num_records);
  last_duration_ = Clock() - beg;
  return results;
}

QueryProcessor::ItemVec QueryProcessor::Intersect(
    const vector<const QueryProcessor::ItemVec*>& lists,
    const int max_num) const {
  using std::make_pair;
  typedef std::priority_queue<std::pair<int, int>, vector<std::pair<int, int> >,
                              std::greater<std::pair<int, int> > > Queue;

  last_num_records_ = 0u;
  const size_t num_lists = lists.size();
  vector<int> indices(num_lists, 0);
  Queue queue;
  for (size_t l = 0; l < num_lists; ++l) {
    queue.push(make_pair((*lists[l])[indices[l]].record_id, l));
  }

  const size_t max_total = num_lists * max_num;
  vector<Index::Item> results;
  results.reserve(max_total);
  vector<Index::Item> matched_items(num_lists);
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
        matched_items[l] = (*lists[l])[indices[l]];
        ++l;
      }
      if (l == num_lists) {
        // Intersection found.
        ++last_num_records_;
        for (auto it = matched_items.cbegin(), end = matched_items.cend();
             it != end; ++it) {
          results.push_back(*it);
        }
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
