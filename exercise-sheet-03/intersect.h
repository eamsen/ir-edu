// Copyright 2012 Eugen Sawin <esawin@me73.com>
#ifndef EXERCISE_SHEET_03_INTERSECT_H_
#define EXERCISE_SHEET_03_INTERSECT_H_

#include <cassert>
#include <vector>
#include <algorithm>
#include <iostream>

namespace es {
  // Linear-time intersection of the two given containers, v1.
  template<typename Container>
  Container IntersectLin1(const Container& list1, const Container& list2) {
    typedef typename Container::value_type value_t;
    static size_t reserve_size_ = 20000000u;

    // Let a be the larger list.
    const Container& a = list1.size() > list2.size() ? list1 : list2;
    const Container& b = list1.size() < list2.size() ? list1 : list2;
    const auto aend = a.cend();
    const auto bend = b.cend();
    auto ait = a.cbegin();
    auto bit = b.cbegin();
    Container result;
    result.reserve(std::min(b.size(),
                            static_cast<size_t>(reserve_size_ * 1.5f)));
    while (ait != aend && bit != bend) {
      const value_t& avalue = *ait;
      const value_t& bvalue = *bit;
      if (avalue == bvalue) {
        result.push_back(avalue);
        ++ait;
        ++bit;
      } else if (avalue < bvalue) {
        ++ait;
      } else {
        ++bit;
      }
    }
    assert(result.size() <= b.size());
    reserve_size_ = (reserve_size_ + result.size()) * 0.5f;
    return result;
  }

  // Linear-time intersection of the two given containers, v2.
  template<typename Container>
  Container IntersectLin2(const Container& list1, const Container& list2) {
    typedef typename Container::value_type value_t;
    // Let's cheat a little (not really, but we save some time testing that way.
    static size_t reserve_size_ = 20000000u;

    // Let a be the larger list.
    const Container& a = list1.size() > list2.size() ? list1 : list2;
    const Container& b = list1.size() < list2.size() ? list1 : list2;
    const auto aend = a.cend();
    const auto bend = b.cend();
    auto ait = a.cbegin();
    auto bit = b.cbegin();
    Container result;
    result.reserve(std::min(b.size(),
                            static_cast<size_t>(reserve_size_ * 1.5f)));
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
        while (ait != aend && bit != bend && *ait == *bit) {
          result.push_back(*ait);
          ++ait;
          ++bit;
        }
      }
    }
    assert(result.size() <= b.size());
    reserve_size_ = (reserve_size_ + result.size()) * 0.5f;
    return result;
  }
}

#endif  // EXERCISE_SHEET_03_INTERSECT_H_
