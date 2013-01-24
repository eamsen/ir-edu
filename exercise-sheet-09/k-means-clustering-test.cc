// Copyright 2012 Eugen Sawin <esawin@me73.com>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>
#include <vector>
#include <set>
#include "./k-means-clustering.h"

using std::vector;
using std::set;
using std::string;
using std::cout;
using std::endl;
using std::ofstream;

using ::testing::ElementsAre;
using ::testing::Contains;
using ::testing::Not;
// using ::testing::WhenSortedBy;

typedef KMeansClustering::IdScore IdScore;

class KMeansClusteringTest : public ::testing::Test {
 public:
  void SetUp() {
  }

  void TearDown() {
  }
};

TEST_F(KMeansClusteringTest, Truncate) {
  vector<IdScore> vec;
  vector<IdScore> res;
  KMeansClustering::Truncate(0, &vec);
  EXPECT_EQ(res, vec);
  KMeansClustering::Truncate(1, &vec);
  EXPECT_EQ(res, vec);
  KMeansClustering::Truncate(10, &vec);
  EXPECT_EQ(res, vec);

  vec = { {0, 1.0f} };
  KMeansClustering::Truncate(0, &vec);
  res = {};
  EXPECT_EQ(res, vec);

  vec = { {0, 1.0f} };
  KMeansClustering::Truncate(1, &vec);
  res = { {0, 1.0f} };
  EXPECT_EQ(res, vec);

  vec = { {0, 0.0f}, {1, 1.0f} };
  KMeansClustering::Truncate(1, &vec);
  res = { {1, 1.0f} };
  EXPECT_EQ(res, vec);

  vec = { {0, 0.0f}, {1, 1.0f}, {2, 2.0f} };
  KMeansClustering::Truncate(1, &vec);
  res = { {2, 2.0f} };
  EXPECT_EQ(res, vec);

  vec = { {0, 0.0f}, {1, 4.0f}, {2, 2.0f} };
  KMeansClustering::Truncate(1, &vec);
  res = { {1, 4.0f} };
  EXPECT_EQ(res, vec);

  vec = { {0, 3.0f}, {1, 1.0f}, {2, 2.0f} };
  KMeansClustering::Truncate(1, &vec);
  res = { {0, 3.0f} };
  EXPECT_EQ(res, vec);

  vec = { {0, 0.0f}, {1, 1.0f}, {2, 2.0f}, {3, 3.0f} };
  KMeansClustering::Truncate(2, &vec);
  res = { {2, 2.0f}, {3, 3.0f} };
  EXPECT_EQ(res, vec);

  vec = { {0, 4.0f}, {1, 1.0f}, {2, 2.0f}, {3, 3.0f} };
  KMeansClustering::Truncate(2, &vec);
  res = { {0, 4.0f}, {3, 3.0f} };
  EXPECT_EQ(res, vec);

  vec = { {0, 4.0f}, {1, 1.0f}, {2, 2.0f}, {3, 0.0f} };
  KMeansClustering::Truncate(2, &vec);
  res = { {0, 4.0f}, {2, 2.0f} };
  EXPECT_EQ(res, vec);

  vec = { {0, 4.0f}, {1, 1.0f}, {2, 2.0f}, {3, 3.0f} };
  KMeansClustering::Truncate(3, &vec);
  res = { {0, 4.0f}, {2, 2.0f}, {3, 3.0f} };
  EXPECT_EQ(res, vec);
}

TEST_F(KMeansClusteringTest, Normalize) {
  vector<IdScore> vec;
  KMeansClustering::Normalize(&vec);
  vector<IdScore> res;
  EXPECT_EQ(res, vec);

  vec = { {0, 0.0f} };
  KMeansClustering::Normalize(&vec);
  res = { {0, 0.0f} };
  EXPECT_EQ(res, vec);

  vec = { {0, 1.0f} };
  KMeansClustering::Normalize(&vec);
  res = { {0, 1.0f} };
  EXPECT_EQ(res, vec);

  vec = { {0, 5.0f} };
  KMeansClustering::Normalize(&vec);
  res = { {0, 1.0f} };
  EXPECT_EQ(res, vec);

  vec = { {0, -5.0f} };
  KMeansClustering::Normalize(&vec);
  res = { {0, -1.0f} };
  EXPECT_EQ(res, vec);

  vec = { {0, 0.0f}, {1, 0.0f}, {2, 0.0f} };
  KMeansClustering::Normalize(&vec);
  res = { {0, 0.0f}, {1, 0.0f}, {2, 0.0f} };
  EXPECT_EQ(res, vec);

  vec = { {0, 1.0f}, {1, 0.0f}, {2, 0.0f} };
  KMeansClustering::Normalize(&vec);
  res = { {0, 1.0f}, {1, 0.0f}, {2, 0.0f} };
  EXPECT_EQ(res, vec);

  vec = { {0, 2.0f}, {1, 0.0f}, {2, 0.0f} };
  KMeansClustering::Normalize(&vec);
  res = { {0, 1.0f}, {1, 0.0f}, {2, 0.0f} };
  EXPECT_EQ(res, vec);

  vec = { {0, -2.0f}, {1, 0.0f}, {2, 0.0f} };
  KMeansClustering::Normalize(&vec);
  res = { {0, -1.0f}, {1, 0.0f}, {2, 0.0f} };
  EXPECT_EQ(res, vec);

  vec = { {0, 2.0f}, {1, 1.0f}, {2, 2.0f} };
  KMeansClustering::Normalize(&vec);
  res = { {0, 2.0f/3.0f}, {1, 1.0f/3.0f}, {2, 2.0f/3.0f} };
  EXPECT_EQ(res, vec);

  vec = { {0, 1.0f}, {1, 1.0f}, {2, 1.0f}, {3, 1.0f} };
  KMeansClustering::Normalize(&vec);
  res = { {0, 0.5f}, {1, 0.5f}, {2, 0.5f}, {3, 0.5f} };
  EXPECT_EQ(res, vec);

  vec = { {0, -1.0f}, {1, 1.0f}, {2, -1.0f}, {3, 1.0f} };
  KMeansClustering::Normalize(&vec);
  res = { {0, -0.5f}, {1, 0.5f}, {2, -0.5f}, {3, 0.5f} };
  EXPECT_EQ(res, vec);
}

TEST_F(KMeansClusteringTest, Distance) {
  vector<IdScore> vec1;
  vector<IdScore> vec2;
  // That's a special case, not really a use case, 0.0f is also a viable result.
  EXPECT_EQ(1.0f, KMeansClustering::Distance(vec1, vec2));

  vec1 = { {0, 1.0f} };
  vec2 = { {0, 1.0f} };
  EXPECT_EQ(0.0f, KMeansClustering::Distance(vec1, vec2));

  vec1 = { {0, 0.5f}, {1, 0.5f}, {2, 0.5f}, {3, 0.5f} };
  vec2 = { {0, 1.0f} };
  EXPECT_EQ(0.5f, KMeansClustering::Distance(vec1, vec2));

  vec1 = { {0, 0.5f}, {1, 0.5f}, {2, 0.5f}, {3, 0.5f} };
  vec2 = { {0, 0.5f}, {1, 0.5f}, {4, 0.5f}, {5, 0.5f} };
  EXPECT_EQ(0.5f, KMeansClustering::Distance(vec1, vec2));

  vec1 = { {0, 0.5f}, {1, 0.5f}, {2, 0.5f}, {3, 0.5f} };
  vec2 = { {0, 0.5f}, {1, 0.5f}, {2, 0.5f}, {5, 0.5f} };
  EXPECT_EQ(0.25f, KMeansClustering::Distance(vec1, vec2));

  vec1 = { {0, 0.5f}, {1, 0.5f}, {2, 0.5f}, {3, 0.5f} };
  vec2 = { {0, 0.5f}, {1, 0.5f}, {2, 0.5f}, {3, 0.5f} };
  EXPECT_EQ(0.0f, KMeansClustering::Distance(vec1, vec2));

  vec1 = { {0, 0.5f}, {1, 0.5f}, {2, 0.5f}, {3, 0.5f} };
  vec2 = { {4, 0.5f}, {5, 0.5f}, {6, 0.5f}, {7, 0.5f} };
  EXPECT_EQ(1.0f, KMeansClustering::Distance(vec1, vec2));
}
