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
  // Linear v0
  EXPECT_EQ(vector<int>({}), IntersectLin0({}, {}));
  EXPECT_EQ(vector<int>({}), IntersectLin0({1}, {}));
  EXPECT_EQ(vector<int>({}), IntersectLin0({1, 2}, {}));
  EXPECT_EQ(vector<int>({}), IntersectLin0({1, 2}, {0}));
  EXPECT_EQ(vector<int>({}), IntersectLin0({1, 2}, {0, 3}));
  EXPECT_EQ(vector<int>({}), IntersectLin0({}, {0, 3}));
  EXPECT_EQ(vector<int>({}), IntersectLin0({}, {3}));
  // Linear v1
  EXPECT_EQ(vector<int>({}), IntersectLin1<vector<int> >({}, {}));
  EXPECT_EQ(vector<int>({}), IntersectLin1<vector<int> >({1}, {}));
  EXPECT_EQ(vector<int>({}), IntersectLin1<vector<int> >({1, 2}, {}));
  EXPECT_EQ(vector<int>({}), IntersectLin1<vector<int> >({1, 2}, {0}));
  EXPECT_EQ(vector<int>({}), IntersectLin1<vector<int> >({1, 2}, {0, 3}));
  EXPECT_EQ(vector<int>({}), IntersectLin1<vector<int> >({}, {0, 3}));
  EXPECT_EQ(vector<int>({}), IntersectLin1<vector<int> >({}, {3}));
  // Linear v2
  EXPECT_EQ(vector<int>({}), IntersectLin2<vector<int> >({}, {}));
  EXPECT_EQ(vector<int>({}), IntersectLin2<vector<int> >({1}, {}));
  EXPECT_EQ(vector<int>({}), IntersectLin2<vector<int> >({1, 2}, {}));
  EXPECT_EQ(vector<int>({}), IntersectLin2<vector<int> >({1, 2}, {0}));
  EXPECT_EQ(vector<int>({}), IntersectLin2<vector<int> >({1, 2}, {0, 3}));
  EXPECT_EQ(vector<int>({}), IntersectLin2<vector<int> >({}, {0, 3}));
  EXPECT_EQ(vector<int>({}), IntersectLin2<vector<int> >({}, {3}));
  // Exponential v0
  EXPECT_EQ(vector<int>({}), IntersectExp0<vector<int> >({}, {}));
  EXPECT_EQ(vector<int>({}), IntersectExp0<vector<int> >({1}, {}));
  EXPECT_EQ(vector<int>({}), IntersectExp0<vector<int> >({1, 2}, {}));
  EXPECT_EQ(vector<int>({}), IntersectExp0<vector<int> >({1, 2}, {0}));
  EXPECT_EQ(vector<int>({}), IntersectExp0<vector<int> >({1, 2}, {0, 3}));
  EXPECT_EQ(vector<int>({}), IntersectExp0<vector<int> >({}, {0, 3}));
  EXPECT_EQ(vector<int>({}), IntersectExp0<vector<int> >({}, {3}));
}

TEST_F(IntersectTest, unexpected) {
  // Linear v0
  EXPECT_EQ(vector<int>({}), IntersectLin0({-1}, {}));
  EXPECT_EQ(vector<int>({}), IntersectLin0({-1}, {-2}));
  EXPECT_EQ(vector<int>({}), IntersectLin0({-1, -2}, {-2}));
  EXPECT_EQ(vector<int>({2}), IntersectLin0({1, 2, 2, 2, 3}, {2}));
  EXPECT_EQ(vector<int>({2, 2}), IntersectLin0({1, 2, 2, 2, 3}, {0, 2, 2}));
  // EXPECT_EQ(vector<int>({}), IntersectLin0({2, 1}, {1, 2}));
  // Linear v1
  EXPECT_EQ(vector<int>({}), IntersectLin1<vector<int> >({-1}, {}));
  EXPECT_EQ(vector<int>({}), IntersectLin1<vector<int> >({-1}, {-2}));
  EXPECT_EQ(vector<int>({}), IntersectLin1<vector<int> >({-1, -2}, {-2}));
  EXPECT_EQ(vector<int>({2}), IntersectLin1<vector<int> >({1, 2, 2, 2}, {2}));
  EXPECT_EQ(vector<int>({2, 2}), IntersectLin1<vector<int> >({1, 2, 2, 2, 3},
                                                             {0, 2, 2}));
  // EXPECT_EQ(vector<int>({}), IntersectLin1<vector<int> >({2, 1}, {1, 2}));
  // Linear v2
  EXPECT_EQ(vector<int>({}), IntersectLin2<vector<int> >({-1}, {}));
  EXPECT_EQ(vector<int>({}), IntersectLin2<vector<int> >({-1}, {-2}));
  EXPECT_EQ(vector<int>({}), IntersectLin2<vector<int> >({-1, -2}, {-2}));
  EXPECT_EQ(vector<int>({2}), IntersectLin2<vector<int> >({1, 2, 2, 2}, {2}));
  EXPECT_EQ(vector<int>({2, 2}), IntersectLin2<vector<int> >({1, 2, 2, 2, 3},
                                                             {0, 2, 2}));
  // EXPECT_EQ(vector<int>({}), IntersectLin2<vector<int> >({2, 1}, {1, 2}));
  // Exponential v0
  EXPECT_EQ(vector<int>({}), IntersectExp0<vector<int> >({-1}, {}));
  EXPECT_EQ(vector<int>({}), IntersectExp0<vector<int> >({-1}, {-2}));
  EXPECT_EQ(vector<int>({}), IntersectExp0<vector<int> >({-1, -2}, {-2}));
  EXPECT_EQ(vector<int>({2}), IntersectExp0<vector<int> >({1, 2, 2, 2}, {2}));
  EXPECT_EQ(vector<int>({2, 2}), IntersectExp0<vector<int> >({1, 2, 2, 2, 3},
                                                             {0, 2, 2}));
  // EXPECT_EQ(vector<int>({}), IntersectExp0<vector<int> >({2, 1}, {1, 2}));
}

TEST_F(IntersectTest, normal) {
  // Linear v0
  EXPECT_EQ(vector<int>({1}), IntersectLin0({1}, {1}));
  EXPECT_EQ(vector<int>({1}), IntersectLin0({1, 2, 10}, {1, 3, 9}));
  EXPECT_EQ(vector<int>({1}), IntersectLin0({0, 1, 2, 10}, {1, 3, 9}));
  EXPECT_EQ(vector<int>({1}), IntersectLin0({1, 2, 10, 11, 12}, {0, 1, 3, 9}));
  EXPECT_EQ(vector<int>({1, 2, 3}), IntersectLin0({1, 2, 3}, {1, 2, 3}));
  EXPECT_EQ(vector<int>({1, 2, 3}), IntersectLin0({0, 1, 2, 3, 4}, {1, 2, 3}));
  EXPECT_EQ(vector<int>({1, 2, 3}),
            IntersectLin0({0, 1, 2, 3, 4}, {1, 2, 3, 5}));
  EXPECT_EQ(vector<int>({1, 20, 30}),
            IntersectLin0({0, 1, 2, 3, 4, 5, 10, 11, 20, 22, 23, 30},
                          {1, 6, 9, 13, 20, 24, 25, 30, 31}));
  // Linear v1
  EXPECT_EQ(vector<int>({1}), IntersectLin1<vector<int> >({1}, {1}));
  EXPECT_EQ(vector<int>({1}),
            IntersectLin1<vector<int> >({1, 2, 10}, {1, 3, 9}));
  EXPECT_EQ(vector<int>({1}),
            IntersectLin1<vector<int> >({0, 1, 2, 10}, {1, 3, 9}));
  EXPECT_EQ(vector<int>({1}),
            IntersectLin1<vector<int> >({1, 2, 10, 11, 12}, {0, 1, 3, 9}));
  EXPECT_EQ(vector<int>({1, 2, 3}),
            IntersectLin1<vector<int> >({1, 2, 3}, {1, 2, 3}));
  EXPECT_EQ(vector<int>({1, 2, 3}),
            IntersectLin1<vector<int> >({0, 1, 2, 3, 4}, {1, 2, 3}));
  EXPECT_EQ(vector<int>({1, 2, 3}),
            IntersectLin1<vector<int> >({0, 1, 2, 3, 4}, {1, 2, 3, 5}));
  EXPECT_EQ(vector<int>({1, 20, 30}),
         IntersectLin1<vector<int> >({0, 1, 2, 3, 4, 5, 10, 11, 20, 22, 23, 30},
                                     {1, 6, 9, 13, 20, 24, 25, 30, 31}));
  // Linear v2
  EXPECT_EQ(vector<int>({1}), IntersectLin2<vector<int> >({1}, {1}));
  EXPECT_EQ(vector<int>({1}),
            IntersectLin2<vector<int> >({1, 2, 10}, {1, 3, 9}));
  EXPECT_EQ(vector<int>({1}),
            IntersectLin2<vector<int> >({0, 1, 2, 10}, {1, 3, 9}));
  EXPECT_EQ(vector<int>({1}),
            IntersectLin2<vector<int> >({1, 2, 10, 11, 12}, {0, 1, 3, 9}));
  EXPECT_EQ(vector<int>({1, 2, 3}),
            IntersectLin2<vector<int> >({1, 2, 3}, {1, 2, 3}));
  EXPECT_EQ(vector<int>({1, 2, 3}),
            IntersectLin2<vector<int> >({0, 1, 2, 3, 4}, {1, 2, 3}));
  EXPECT_EQ(vector<int>({1, 2, 3}),
            IntersectLin2<vector<int> >({0, 1, 2, 3, 4}, {1, 2, 3, 5}));
  EXPECT_EQ(vector<int>({1, 20, 30}),
         IntersectLin2<vector<int> >({0, 1, 2, 3, 4, 5, 10, 11, 20, 22, 23, 30},
                                     {1, 6, 9, 13, 20, 24, 25, 30, 31}));
  // Exponential v0
  EXPECT_EQ(vector<int>({1}), IntersectExp0<vector<int> >({1}, {1}));
  EXPECT_EQ(vector<int>({1}),
            IntersectExp0<vector<int> >({1, 2, 10}, {1, 3, 9}));
  EXPECT_EQ(vector<int>({1}),
            IntersectExp0<vector<int> >({0, 1, 2, 10}, {1, 3, 9}));
  EXPECT_EQ(vector<int>({1}),
            IntersectExp0<vector<int> >({1, 2, 10, 11, 12}, {0, 1, 3, 9}));
  EXPECT_EQ(vector<int>({1, 2, 3}),
            IntersectExp0<vector<int> >({1, 2, 3}, {1, 2, 3}));
  EXPECT_EQ(vector<int>({1, 2, 3}),
            IntersectExp0<vector<int> >({0, 1, 2, 3, 4}, {1, 2, 3}));
  EXPECT_EQ(vector<int>({1, 2, 3}),
            IntersectExp0<vector<int> >({0, 1, 2, 3, 4}, {1, 2, 3, 5}));
  EXPECT_EQ(vector<int>({1, 20, 30}),
         IntersectExp0<vector<int> >({0, 1, 2, 3, 4, 5, 10, 11, 20, 22, 23, 30},
                                     {1, 6, 9, 13, 20, 24, 25, 30, 31}));
}
