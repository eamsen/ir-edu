// Copyright 2012 Eugen Sawin <esawin@me73.com>
#ifndef EXERCISE_SHEET_03_INTERSECT_H_
#define EXERCISE_SHEET_03_INTERSECT_H_

#include <cassert>
#include <vector>
#include <algorithm>
#include <iostream>

// Linear-time intersection of the two given containers, v0.
std::vector<int> IntersectLin0(const std::vector<int>& a,
                               const std::vector<int>& b) {
  std::vector<int> result;
  const size_t asize = a.size();
  const size_t bsize = b.size();
  if (asize < bsize) {
    // Let a be the larger list.
    return IntersectLin0(b, a);
  }
  size_t ai = 0u;
  size_t bi = 0u;
  result.reserve(bsize);
  while (ai < asize && bi < bsize) {
    while (ai < asize && a[ai] < b[bi]) {
      ++ai;
    }
    if (ai < asize) {
      while (bi < bsize && b[bi] < a[ai]) {
        ++bi;
      }
      while (ai < asize && bi < bsize && a[ai] == b[bi]) {
        result.push_back(a[ai]);
        ++ai;
        ++bi;
      }
    }
  }
  assert(result.size() <= bsize);
  return result;
}

// Linear-time intersection of the two given containers, v1.
template<class InputIt1, class InputIt2, class OutputIt>
OutputIt IntersectLin1(InputIt1 first1, InputIt1 end1,
                       InputIt2 first2, InputIt2 end2,
                       OutputIt result) {
  // We might drop that to support non-sequenced containers.
  if (end1 - first1 < end2 - first2) {
    // Let first container be the larger one.
    return IntersectLin1(first2, end2, first1, end1, result);
  }
  while (first1 != end1 && first2 != end2) {
    if (*first1 < *first2) {
      ++first1;
    } else if (*first2 < *first1) {
      ++first2;
    } else {
      *result++ = *first1;
      ++first1;
      ++first2;
    }
  }
  return result;
}

// Linear-time intersection of the two given containers, v2.
template<class InputIt1, class InputIt2, class OutputIt>
OutputIt IntersectLin2(InputIt1 first1, InputIt1 end1,
                       InputIt2 first2, InputIt2 end2,
                       OutputIt result) {
  // We might drop that to support non-sequenced containers.
  if (end1 - first1 < end2 - first2) {
    // Let first container be the larger one.
    return IntersectLin2(first2, end2, first1, end1, result);
  }
  while (first1 != end1 && first2 != end2) {
    while (first1 != end1 && *first1 < *first2) {
      ++first1;
    }
    if (first1 == end1) {
      break;
    }
    while (first2 != end2 && *first2 < *first1) {
      ++first2;
    }
    while (first1 != end1 && first2 != end2 && *first1 == *first2) {
      *result++ = *first1;
      ++first1;
      ++first2;
    }
  }
  return result;
}

// Exponential binary-search intersection of the two given containers, v0.
template<class InputIt1, class InputIt2, class OutputIt>
OutputIt IntersectExp0(InputIt1 first1, InputIt1 end1,
                       InputIt2 first2, InputIt2 end2,
                       OutputIt result) {
  // We might drop that to support non-sequenced containers.
  if (end1 - first1 > end2 - first2) {
    // Let first container be the smaller one.
    return IntersectExp0(first2, end2, first1, end1, result);
  }
  for (; first1 != end1; ++first1) {
    auto search_end = first2;
    // Find end index by exponential search.
    size_t exponent = 1u;
    while (search_end != end2 && *search_end < *first1) {
      first2 = search_end;
      // Supported by random access iterators only.
      search_end = std::min(end2, search_end + exponent);
      exponent *= 2u;
    }
    // Find match and next start index by binary search.
    first2 = std::lower_bound(first2, search_end, *first1);
    if (first2 == end2) {
      // Reached end of second list, no more matches.
      break;
    } else if (*first2 == *first1) {
      // Found a match.
      *result++ = *first1;
    }
  }
  return result;
}

#endif  // EXERCISE_SHEET_03_INTERSECT_H_
