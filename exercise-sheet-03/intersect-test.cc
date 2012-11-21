// Copyright 2012 Eugen Sawin <esawin@me73.com>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>
#include <vector>
#include <set>
#include "./intersect.h"

using std::vector;
using std::set;

using ::testing::ElementsAre;
using ::testing::Contains;
using ::testing::Not;
// using ::testing::WhenSortedBy;

class IntersectTest : public ::testing::Test {
 public:
  void SetUp() {}

  void TearDown() {}
};

TEST_F(IntersectTest, boundary) {
  vector<int> empty;
  vector<int> zero = {0};
  vector<int> one = {1};
  vector<int> three = {3};
  vector<int> onetwo = {1, 2};
  vector<int> zerothree = {0, 3};

  // Linear v0
  EXPECT_EQ(empty, IntersectLin0(empty, empty));
  EXPECT_EQ(empty, IntersectLin0(one, empty));
  EXPECT_EQ(empty, IntersectLin0(onetwo, empty));
  EXPECT_EQ(empty, IntersectLin0(onetwo, zero));
  EXPECT_EQ(empty, IntersectLin0(onetwo, zerothree));
  EXPECT_EQ(empty, IntersectLin0(empty, zerothree));
  EXPECT_EQ(empty, IntersectLin0(empty, three));
  // Linear v1
  {
    vector<int> result(2);
    auto end = IntersectLin1(empty.begin(), empty.end(),
                             empty.begin(), empty.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(empty, result);
  }
  {
    vector<int> result(2);
    auto end = IntersectLin1(one.begin(), one.end(),
                             empty.begin(), empty.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(empty, result);
  }
  {
    vector<int> result(2);
    auto end = IntersectLin1(onetwo.begin(), onetwo.end(),
                             empty.begin(), empty.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(empty, result);
  }
  {
    vector<int> result(2);
    auto end = IntersectLin1(onetwo.begin(), onetwo.end(),
                             zero.begin(), zero.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(empty, result);
  }
  {
    vector<int> result(2);
    auto end = IntersectLin1(onetwo.begin(), onetwo.end(),
                             zerothree.begin(), zerothree.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(empty, result);
  }
  {
    vector<int> result(2);
    auto end = IntersectLin1(empty.begin(), empty.end(),
                             zerothree.begin(), zerothree.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(empty, result);
  }
  {
    vector<int> result(2);
    auto end = IntersectLin1(empty.begin(), empty.end(),
                             three.begin(), three.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(empty, result);
  }
  // Linear v2
  {
    vector<int> result(2);
    auto end = IntersectLin2(empty.begin(), empty.end(),
                             empty.begin(), empty.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(empty, result);
  }
  {
    vector<int> result(2);
    auto end = IntersectLin2(one.begin(), one.end(),
                             empty.begin(), empty.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(empty, result);
  }
  {
    vector<int> result(2);
    auto end = IntersectLin2(onetwo.begin(), onetwo.end(),
                             empty.begin(), empty.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(empty, result);
  }
  {
    vector<int> result(2);
    auto end = IntersectLin2(onetwo.begin(), onetwo.end(),
                             zero.begin(), zero.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(empty, result);
  }
  {
    vector<int> result(2);
    auto end = IntersectLin2(onetwo.begin(), onetwo.end(),
                             zerothree.begin(), zerothree.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(empty, result);
  }
  {
    vector<int> result(2);
    auto end = IntersectLin2(empty.begin(), empty.end(),
                             zerothree.begin(), zerothree.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(empty, result);
  }
  {
    vector<int> result(2);
    auto end = IntersectLin2(empty.begin(), empty.end(),
                             three.begin(), three.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(empty, result);
  }
  // Exponential v0
  {
    vector<int> result(2);
    auto end = IntersectExp0(empty.begin(), empty.end(),
                             empty.begin(), empty.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(empty, result);
  }
  {
    vector<int> result(2);
    auto end = IntersectExp0(one.begin(), one.end(),
                             empty.begin(), empty.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(empty, result);
  }
  {
    vector<int> result(2);
    auto end = IntersectExp0(onetwo.begin(), onetwo.end(),
                             empty.begin(), empty.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(empty, result);
  }
  {
    vector<int> result(2);
    auto end = IntersectExp0(onetwo.begin(), onetwo.end(),
                             zero.begin(), zero.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(empty, result);
  }
  {
    vector<int> result(2);
    auto end = IntersectExp0(onetwo.begin(), onetwo.end(),
                             zerothree.begin(), zerothree.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(empty, result);
  }
  {
    vector<int> result(2);
    auto end = IntersectExp0(empty.begin(), empty.end(),
                             zerothree.begin(), zerothree.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(empty, result);
  }
  {
    vector<int> result(2);
    auto end = IntersectExp0(empty.begin(), empty.end(),
                             three.begin(), three.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(empty, result);
  }
}

TEST_F(IntersectTest, unexpected) {
  vector<int> empty;
  vector<int> v1 = {-1};
  vector<int> v2 = {-2};
  vector<int> v3 = {-1, -2};
  vector<int> v4 = {1, 2, 2, 2, 3};
  vector<int> v5 = {2};
  vector<int> v6 = {0, 2, 2};
  vector<int> v7 = {1, 2, 2, 2};
  vector<int> v4_v5 = {2};
  vector<int> v4_v6 = {2, 2};
  vector<int> v5_v7 = {2};

  // Linear v0
  EXPECT_EQ(empty, IntersectLin0(v1, empty));
  EXPECT_EQ(empty, IntersectLin0(v1, v2));
  EXPECT_EQ(empty, IntersectLin0(v3, v2));
  EXPECT_EQ(v5, IntersectLin0(v4, v5));
  EXPECT_EQ(v4_v6, IntersectLin0(v4, v6));
  // EXPECT_EQ(empty, IntersectLin0({2, 1}, {1, 2}));
  // Linear v1
  {
    vector<int> result(2);
    auto end = IntersectLin1(v1.begin(), v1.end(),
                             empty.begin(), empty.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(empty, result);
  }
  {
    vector<int> result(2);
    auto end = IntersectLin1(v1.begin(), v1.end(),
                             v2.begin(), v2.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(empty, result);
  }
  {
    vector<int> result(2);
    auto end = IntersectLin1(v3.begin(), v3.end(),
                             v2.begin(), v2.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(empty, result);
  }
  {
    vector<int> result(2);
    auto end = IntersectLin1(v7.begin(), v7.end(),
                             v5.begin(), v5.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(v5, result);
  }
  {
    vector<int> result(2);
    auto end = IntersectLin1(v4.begin(), v4.end(),
                             v6.begin(), v6.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(v4_v6, result);
  }
  // EXPECT_EQ(empty, IntersectLin1<vector<int> >({2, 1}, {1, 2}));
  // Linear v2
  {
    vector<int> result(2);
    auto end = IntersectLin1(v1.begin(), v1.end(),
                             empty.begin(), empty.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(empty, result);
  }
  {
    vector<int> result(2);
    auto end = IntersectLin1(v1.begin(), v1.end(),
                             v2.begin(), v2.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(empty, result);
  }
  {
    vector<int> result(2);
    auto end = IntersectLin1(v3.begin(), v3.end(),
                             v2.begin(), v2.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(empty, result);
  }
  {
    vector<int> result(2);
    auto end = IntersectLin1(v7.begin(), v7.end(),
                             v5.begin(), v5.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(v5_v7, result);
  }
  {
    vector<int> result(2);
    auto end = IntersectLin1(v4.begin(), v4.end(),
                             v6.begin(), v6.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(v4_v6, result);
  }
  // EXPECT_EQ(empty, IntersectLin2<vector<int> >({2, 1}, {1, 2}));
  // Exponential v0
  {
    vector<int> result(2);
    auto end = IntersectLin1(v1.begin(), v1.end(),
                             empty.begin(), empty.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(empty, result);
  }
  {
    vector<int> result(2);
    auto end = IntersectLin1(v1.begin(), v1.end(),
                             v2.begin(), v2.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(empty, result);
  }
  {
    vector<int> result(2);
    auto end = IntersectLin1(v3.begin(), v3.end(),
                             v2.begin(), v2.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(empty, result);
  }
  {
    vector<int> result(2);
    auto end = IntersectLin1(v7.begin(), v7.end(),
                             v5.begin(), v5.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(v5_v7, result);
  }
  {
    vector<int> result(2);
    auto end = IntersectLin1(v4.begin(), v4.end(),
                             v6.begin(), v6.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(v4_v6, result);
  }
  // EXPECT_EQ(empty, IntersectExp0<vector<int> >({2, 1}, {1, 2}));
}

TEST_F(IntersectTest, normal) {
  vector<int> v01 = {1};
  vector<int> v02 = {1, 2, 10};
  vector<int> v03 = {1, 3, 9};
  vector<int> v04 = {0, 1, 2, 10};
  // Missing variable, that's ok.
  vector<int> v06 = {1, 2, 10, 11, 12};
  vector<int> v07 = {0, 1, 3, 9};
  vector<int> v08 = {1, 2, 3};
  vector<int> v09 = {0, 1, 2, 3, 4};
  vector<int> v10 = {1, 2, 3, 5};
  vector<int> v11 = {0, 1, 2, 3, 4, 5, 10, 11, 20, 22, 23, 30};
  vector<int> v12 = {1, 6, 9, 13, 20, 24, 25, 30, 31};
  vector<int> v02_v03 = {1};
  vector<int> v03_v04 = {1};
  vector<int> v06_v07 = {1};
  vector<int> v08_v07 = {1};
  vector<int> v08_v09 = {1, 2, 3};
  vector<int> v09_v10 = {1, 2, 3};
  vector<int> v11_v12 = {1, 20, 30};
  // Linear v0
  EXPECT_EQ(v01, IntersectLin0(v01, v01));
  EXPECT_EQ(v02_v03, IntersectLin0(v02, v03));
  EXPECT_EQ(v03_v04, IntersectLin0(v04, v03));
  EXPECT_EQ(v06_v07, IntersectLin0(v06, v07));
  EXPECT_EQ(v08, IntersectLin0(v08, v08));
  EXPECT_EQ(v08_v09, IntersectLin0(v09, v08));
  EXPECT_EQ(v09_v10, IntersectLin0(v09, v10));
  EXPECT_EQ(v11_v12, IntersectLin0(v11, v12));
  // Linear v1
  {
    vector<int> result(3);
    auto end = IntersectLin1(v01.begin(), v01.end(),
                             v01.begin(), v01.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(v01, result);
  }
  {
    vector<int> result(3);
    auto end = IntersectLin1(v02.begin(), v02.end(),
                             v03.begin(), v03.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(v02_v03, result);
  }
  {
    vector<int> result(3);
    auto end = IntersectLin1(v02.begin(), v02.end(),
                             v03.begin(), v03.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(v02_v03, result);
  }
  {
    vector<int> result(3);
    auto end = IntersectLin1(v04.begin(), v04.end(),
                             v03.begin(), v03.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(v03_v04, result);
  }
  {
    vector<int> result(3);
    auto end = IntersectLin1(v06.begin(), v06.end(),
                             v07.begin(), v07.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(v06_v07, result);
  }
  {
    vector<int> result(3);
    auto end = IntersectLin1(v08.begin(), v08.end(),
                             v08.begin(), v08.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(v08, result);
  }
  {
    vector<int> result(3);
    auto end = IntersectLin1(v09.begin(), v09.end(),
                             v08.begin(), v08.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(v08_v09, result);
  }
  {
    vector<int> result(3);
    auto end = IntersectLin1(v09.begin(), v09.end(),
                             v10.begin(), v10.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(v09_v10, result);
  }
  {
    vector<int> result(3);
    auto end = IntersectLin1(v11.begin(), v11.end(),
                             v12.begin(), v12.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(v11_v12, result);
  }
  // Linear v2
  {
    vector<int> result(3);
    auto end = IntersectLin2(v01.begin(), v01.end(),
                             v01.begin(), v01.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(v01, result);
  }
  {
    vector<int> result(3);
    auto end = IntersectLin2(v02.begin(), v02.end(),
                             v03.begin(), v03.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(v02_v03, result);
  }
  {
    vector<int> result(3);
    auto end = IntersectLin2(v02.begin(), v02.end(),
                             v03.begin(), v03.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(v02_v03, result);
  }
  {
    vector<int> result(3);
    auto end = IntersectLin2(v04.begin(), v04.end(),
                             v03.begin(), v03.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(v03_v04, result);
  }
  {
    vector<int> result(3);
    auto end = IntersectLin2(v06.begin(), v06.end(),
                             v07.begin(), v07.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(v06_v07, result);
  }
  {
    vector<int> result(3);
    auto end = IntersectLin2(v08.begin(), v08.end(),
                             v08.begin(), v08.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(v08, result);
  }
  {
    vector<int> result(3);
    auto end = IntersectLin2(v09.begin(), v09.end(),
                             v08.begin(), v08.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(v08_v09, result);
  }
  {
    vector<int> result(3);
    auto end = IntersectLin2(v09.begin(), v09.end(),
                             v10.begin(), v10.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(v09_v10, result);
  }
  {
    vector<int> result(3);
    auto end = IntersectLin2(v11.begin(), v11.end(),
                             v12.begin(), v12.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(v11_v12, result);
  }
  // Exponential v0
  {
    vector<int> result(3);
    auto end = IntersectExp0(v01.begin(), v01.end(),
                             v01.begin(), v01.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(v01, result);
  }
  {
    vector<int> result(3);
    auto end = IntersectExp0(v02.begin(), v02.end(),
                             v03.begin(), v03.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(v02_v03, result);
  }
  {
    vector<int> result(3);
    auto end = IntersectExp0(v02.begin(), v02.end(),
                             v03.begin(), v03.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(v02_v03, result);
  }
  {
    vector<int> result(3);
    auto end = IntersectExp0(v04.begin(), v04.end(),
                             v03.begin(), v03.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(v03_v04, result);
  }
  {
    vector<int> result(3);
    auto end = IntersectExp0(v06.begin(), v06.end(),
                             v07.begin(), v07.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(v06_v07, result);
  }
  {
    vector<int> result(3);
    auto end = IntersectExp0(v08.begin(), v08.end(),
                             v08.begin(), v08.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(v08, result);
  }
  {
    vector<int> result(3);
    auto end = IntersectExp0(v09.begin(), v09.end(),
                             v08.begin(), v08.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(v08_v09, result);
  }
  {
    vector<int> result(3);
    auto end = IntersectExp0(v09.begin(), v09.end(),
                             v10.begin(), v10.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(v09_v10, result);
  }
  {
    vector<int> result(3);
    auto end = IntersectExp0(v11.begin(), v11.end(),
                             v12.begin(), v12.end(),
                             result.begin());
    result.resize(end - result.begin());
    EXPECT_EQ(v11_v12, result);
  }
}
