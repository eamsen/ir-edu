// Copyright 2012 Eugen Sawin <esawin@me73.com>
#ifndef EXERCISE_01_QUERY_PROCESSOR_H_
#define EXERCISE_01_QUERY_PROCESSOR_H_

#include <string>
#include <vector>
#include "./inverted-index.h"

class Index;

// Class for processing queries with two keywords based on an inverted index.
class QueryProcessor {
 public:
  // Initializes the query processor for given index.
  explicit QueryProcessor(const Index& index);

  // Returns the matching record ids for given query.
  std::vector<Index::Item> Answer(const std::string& query,
                          const int max_num_records) const;

 private:
  typedef std::vector<Index::Item> ItemVec;

  // Intersects inverted lists and returns the result list.
  static ItemVec Intersect(const std::vector<const ItemVec*>& lists,
                           const int max_num);

  const Index& index_;
};

#endif  // EXERCISE_01_QUERY_PROCESSOR_H_
