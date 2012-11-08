// Copyright 2012 Eugen Sawin <esawin@me73.com>
#ifndef EXERCISE_SHEET_03_INTERSECT_H_
#define EXERCISE_SHEET_03_INTERSECT_H_

#include <cassert>
#include <vector>
#include <algorithm>

namespace es {
  // Linear-time intersection of the two given containers.
  template<typename Container>
  Container IntersectLin(const Container& list1, const Container& list2) {
    typedef typename Container::value_type value_t;

    // Let a be the smaller list.
    const Container& a = list1.size() < list2.size() ? list1 : list2;
    const Container& b = list1.size() > list2.size() ? list1 : list2;
    const auto aend = a.cend();
    const auto bend = b.cend();
    auto ait = a.cbegin();
    auto bit = b.cbegin();
    Container result(std::min(a.size(), b.size()));
    size_t size = 0u;
    while (ait != aend && bit != bend) {
      const value_t& bvalue = *bit;
      while (ait != aend && *ait < bvalue) {
        ++ait;
      }
      if (ait != aend) {
        const value_t& avalue = *ait;
        while (bit != bend && *bit < avalue) {
          ++bit;
        }
        if (bit != bend && avalue == *bit) {
          result[size++] = avalue;
          ++ait;
          ++bit;
        }
      }
    }
    result.resize(size);
    return result;
  }
}

#endif  // EXERCISE_SHEET_03_INTERSECT_H_
