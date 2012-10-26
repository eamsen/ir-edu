// Copyright 2012, University of Freiburg,
#ifndef EXERCISE_01_QUERY_PROCESSOR_H_
#define EXERCISE_01_QUERY_PROCESSOR_H_
// Chair of Algorithms and Data Structures.
// Author: Hannah Bast <bast@informatik.uni-freiburg.de>.

// NOTE: this is a code design suggestion in pseudo-code. It is not supposed to
// be compilable in any language. You have to translate it to Java or C++
// yourself.

// Class for processing queries with two keywords based on an inverted index.
class QueryProcessor {
  // PUBLIC MEMBERS.

  // Construct for given inverted index. Make sure that the class only stores a
  // reference to the given index, and not a copy!
  // QueryProcessor(Index index);

  // Answer given query. Return URLs and contents of k matching records (or less
  // if less records match), one string per record.
  // Note: you can also return a pair of strings for each record, that might be
  // nicer for the calling method.
  // Array<String> void answerQuery(String query, int k);

  // PRIVATE MEMBERS.

  // Intersect two inverted lists and return the result list.
  // Array<int> intersect(Array<int> list1, Array<int> list2);

  // The inverted index.
  // Index index.
}
#endif  // EXERCISE_01_QUERY_PROCESSOR_H_
