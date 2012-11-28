// Copyright 2012 Eugen Sawin <esawin@me73.com>
#ifndef EXERCISE_SHEET_05_QUERY_PROCESSOR_H_
#define EXERCISE_SHEET_05_QUERY_PROCESSOR_H_

#include <string>
#include <vector>
#include "./index.h"
#include "./clock.h"

// Query processor based on an inverted index.
class QueryProcessor {
 public:
  // Initializes the query processor for given index.
  explicit QueryProcessor(const Index& index);

  // Returns the best matching record ids for given query.
  // The items are sorted by score in reversed order. There is one item per
  // record for each keyword considered.
  std::vector<Index::Item> Answer(const std::string& query,
                                  const size_t max_num_records) const;

  // Returns the best matching items ranked by the score.
  // The result is sorted by score in reversed order.
  // The number of keywords parameter is only used as a hint for efficiency.
  std::vector<Index::Item> Rank(const std::vector<Index::Item>& items,
                                const size_t max_num_records,
                                const size_t num_keywords) const;

  // Returns the number of records found in the last call to Answer.
  size_t LastRecordsFound() const;

  // Returns the duration of the last query processing in microseconds.
  Clock::Diff LastDuration() const;

 private:
  // Intersects inverted lists and returns the result list.
  std::vector<Index::Item> Intersect(
      const std::vector<const std::vector<Index::Item>*>& lists) const;

  const Index& index_;
  mutable size_t last_num_records_;
  mutable Clock::Diff last_duration_;
};

#endif  // EXERCISE_SHEET_05_QUERY_PROCESSOR_H_
