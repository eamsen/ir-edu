// Copyright 2012 Eugen Sawin <esawin@me73.com>
#ifndef EXERCISE_SHEET_03_INTERSECT_H_
#define EXERCISE_SHEET_03_INTERSECT_H_

#include <cassert>
#include <vector>
#include <algorithm>
#include <iostream>

// Linear-time intersection of the two given containers, v1.
template<typename Container>
Container IntersectLin1(const Container& list1, const Container& list2) {
  // Let a be the larger list.
  const Container& a = list1.size() > list2.size() ? list1 : list2;
  const Container& b = list1.size() < list2.size() ? list1 : list2;
  const auto aend = a.cend();
  const auto bend = b.cend();
  auto ait = a.cbegin();
  auto bit = b.cbegin();
  Container result;
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
  // Let a be the larger list.
  const Container& a = list1.size() > list2.size() ? list1 : list2;
  const Container& b = list1.size() < list2.size() ? list1 : list2;
  const auto aend = a.cend();
  const auto bend = b.cend();
  auto ait = a.cbegin();
  auto bit = b.cbegin();
  Container result;
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

// Linear-time intersection of the two given containers, v3.
std::vector<int> IntersectLin3(const std::vector<int>& list1,
                               const std::vector<int>& list2) {
  // Let a be the larger list.
  const std::vector<int>& a = list1.size() > list2.size() ? list1 : list2;
  const std::vector<int>& b = list1.size() < list2.size() ? list1 : list2;
  const size_t asize = a.size();
  const size_t bsize = b.size();
  size_t ai = 0u;
  size_t bi = 0u;
  std::vector<int> result;
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

// Exponential binary-search intersection of the two given containers, v1.
template<typename Container>
Container IntersectExp1(const Container& list1, const Container& list2) {
  // Let a be the smaller list.
  const Container& a = list1.size() < list2.size() ? list1 : list2;
  const Container& b = list1.size() > list2.size() ? list1 : list2;
  const auto aend = a.cend();
  const auto bend = b.cend();
  Container result;
  result.reserve(a.size());
  auto ait = a.cbegin();
  auto search_beg = b.cbegin();
  while (ait != aend && search_beg != bend) {
    // Find end index by exponential search.
    size_t f = 1u;
    auto search_end = search_beg;
    while (search_end != bend && *search_end <= *ait) {
      // Supported by random access iterators only.
      search_end = std::min(bend, search_end + f);
      f *= 2u;
    }
    // Find match and next start index by binary search.
    search_beg = std::lower_bound(search_beg, search_end, *ait);
    if (search_beg != search_end) {
      if (*search_beg == *ait) {
        result.push_back(*ait);
      }
      ++ait;
    }
  }
  assert(result.size() <= a.size());
  return result;
}

#endif  // EXERCISE_SHEET_03_INTERSECT_H_
