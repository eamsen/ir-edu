// Copyright 2012 Eugen Sawin <esawin@me73.com>
#ifndef EXERCISE_SHEET_03_INTERSECT_H_
#define EXERCISE_SHEET_03_INTERSECT_H_

#include <cassert>
#include <vector>
#include <algorithm>
#include <iostream>

// Linear-time intersection of the two given containers, v0.
std::vector<int> IntersectLin0(const std::vector<int>& list1,
                               const std::vector<int>& list2) {
  std::vector<int> result;
  // Let a be the larger list.
  const std::vector<int>& a = list1.size() >= list2.size() ? list1 : list2;
  const std::vector<int>& b = list1.size() < list2.size() ? list1 : list2;
  const size_t asize = a.size();
  const size_t bsize = b.size();
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
template<typename Container>
Container IntersectLin1(const Container& list1, const Container& list2) {
  Container result;
  // Let a be the larger list.
  const Container& a = list1.size() >= list2.size() ? list1 : list2;
  const Container& b = list1.size() < list2.size() ? list1 : list2;
  const auto aend = a.cend();
  const auto bend = b.cend();
  auto ait = a.cbegin();
  auto bit = b.cbegin();
  result.reserve(b.size());
  while (ait != aend && bit != bend) {
    if (*ait < *bit) {
      ++ait;
    } else if (*bit < *ait) {
      ++bit;
    } else {
      result.push_back(*ait);
      ++ait;
      ++bit;
    }
  }
  assert(result.size() <= b.size());
  return result;
}

// Linear-time intersection of the two given containers, v2.
template<typename Container>
Container IntersectLin2(const Container& list1, const Container& list2) {
  Container result;
  // Let a be the larger list.
  const Container& a = list1.size() >= list2.size() ? list1 : list2;
  const Container& b = list1.size() < list2.size() ? list1 : list2;
  const auto aend = a.cend();
  const auto bend = b.cend();
  auto ait = a.cbegin();
  auto bit = b.cbegin();
  result.reserve(b.size());
  while (ait != aend && bit != bend) {
    while (ait != aend && *ait < *bit) {
      ++ait;
    }
    if (ait != aend) {
      while (bit != bend && *bit < *ait) {
        ++bit;
      }
      while (ait != aend && bit != bend && *ait == *bit) {
        result.push_back(*ait);
        ++ait;
        ++bit;
      }
    }
  }
  assert(result.size() <= b.size());
  return result;
}

// Exponential binary-search intersection of the two given containers, v0.
template<typename Container>
Container IntersectExp0(const Container& list1, const Container& list2) {
  typedef typename Container::value_type value_t;

  Container result;
  // Let a be the smaller list.
  const Container& a = list1.size() <= list2.size() ? list1 : list2;
  const Container& b = list1.size() > list2.size() ? list1 : list2;
  result.reserve(a.size());
  auto search_beg = b.cbegin();
  const auto bend = b.cend();
  for (const value_t& value: a) {
    // Find end index by exponential search.
    size_t exponent = 1u;
    auto search_end = search_beg;
    while (search_end != bend && *search_end <= value) {
      // Supported by random access iterators only.
      search_end = std::min(bend, search_end + exponent);
      exponent *= 2u;
    }
    // Find match and next start index by binary search.
    search_beg = std::lower_bound(search_beg, search_end, value);
    if (search_beg == bend) {
      // No more matches.
      break;
    } else if (*search_beg == value) {
      // Found a match.
      result.push_back(value);
    }
  }
  assert(result.size() <= a.size());
  return result;
}

#endif  // EXERCISE_SHEET_03_INTERSECT_H_
