// Copyright 2012 Eugen Sawin <esawin@me73.com>
#ifndef EXERCISE_SHEET_03_INTERSECT_H_
#define EXERCISE_SHEET_03_INTERSECT_H_

#include <cassert>
#include <vector>
#include <algorithm>

namespace es {
  template<typename Container>
  Container IntersectLin(const Container& a, const Container& b) {
    const auto aend = a.cend();
    const auto bend = b.cend();
    auto ait = a.cbegin();
    auto bit = b.cbegin();
    Container result(std::min(a.size(), b.size()));
    size_t size = 0u;
    while (ait != aend && bit != bend) {
      if (*ait == *bit) {
        result[size++] = *ait++;
        ++bit;
      } else {
        while (*ait < *bit && ait != aend) {
          ++ait;
        }
        while (*ait > *bit && bit != bend) {
          ++bit;
        }
      }
    }
    result.resize(size);
    return result;
  }
}

#endif  // EXERCISE_SHEET_03_INTERSECT_H_
