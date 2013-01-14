// Copyright 2013 Eugen Sawin <esawin@me73.com>
#include "./k-means-clustering.h"
#include <cassert>
#include <algorithm>
#include <iostream>
#include "./index.h"

using std::vector;

const int KMeansClustering::kInvalidId = -1;

KMeansClustering::KMeansClustering(const Index& index)
    : index_(index) {}

void KMeansClustering::Truncate(const size_t m, vector<IdScore>* vec) {
  if (vec->size() <= m) {
    return;
  }
  std::partial_sort(vec->begin(), vec->begin() + m, vec->end(),
      [](const IdScore& lhs, const IdScore& rhs) {
    return lhs.score > rhs.score || (lhs.score == rhs.score && lhs < rhs);
  });
  vec->resize(m);
  std::sort(vec->begin(), vec->end());
}

float Norm(const vector<KMeansClustering::IdScore>& vec) {
  float sum2 = 0.0f;
  for (const KMeansClustering::IdScore& idsc: vec) {
    sum2 += idsc.score * idsc.score;
  }
  return std::sqrt(sum2);
}

// vector<IdScore> DiffSq(const vector<IdScore>& vec1,
//                        const vector<IdScore>& vec2) {
//   auto it1 = vec1.begin();
//   const auto end1 = vec1.end();
//   auto it2 = vec2.begin();
//   const auto end2 = vec2.end();
//   vector<IdScore> result;
//   result.reserve(vec1.size() + vec2.size());
//   while (it1 != end1 && it2 != end2) {
//     while (it1 != end1 && *it1 < *it2) {
//       result.push_back(it1->score * it1->score);
//       ++it1;
//     }
//     if (it1 == end1) {
//       break;
//     }
//     while (it2 != end2 && *it2 < *it1) {
//       result.push_back(it2->score * it2->score);
//       ++it2;
//     }
//     while (it1 != end1 && it2 != end2 && *it1 == *it2) {
//       const float diff = it1->score - it2->score;
//       result.push_back(diff * diff);
//       ++it1;
//       ++it2;
//     }
//   }
//   return result;
// }


void KMeansClustering::Normalize(vector<IdScore>* vec) {
  const float norm = Norm(*vec);
  if (norm == 0.0f) {
    return;
  }
  for (IdScore& idsc: *vec) {
    idsc.score /= norm;
  }
}

float KMeansClustering::Distance(const vector<IdScore>& v1,
                                 const vector<IdScore>& v2) const {
  auto it1 = v1.begin();
  const auto end1 = v1.end();
  auto it2 = v2.begin();
  const auto end2 = v2.end();
  float dist = 1.0f;
  while (it1 != end1 && it2 != end2) {
    while (it1 != end1 && *it1 < *it2) {
      ++it1;
    }
    if (it1 == end1) {
      break;
    }
    while (it2 != end2 && *it2 < *it1) {
      ++it2;
    }
    while (it1 != end1 && it2 != end2 && *it1 == *it2) {
      dist -= it1->score * it2->score;
      ++it1;
      ++it2;
    }
  }
  return dist;
}

auto KMeansClustering::Average(const vector<int>& records) const
    -> vector<IdScore> {
  vector<IdScore> merged;
  for (const int id: records) {
    const vector<IdScore>& vec = RecordVector(id);
    merged.insert(merged.end(), vec.begin(), vec.end());
  }
  std::sort(merged.begin(), merged.end());
  vector<IdScore> avg;
  for (const IdScore& idsc: merged) {
    if (avg.size() && avg.back().id == idsc.id) {
      // Same record, increase sum.
      avg.back().score += idsc.score;
    } else {
      avg.push_back(idsc);
    }
  }
  const int num_records = records.size();
  for (IdScore& idsc: avg) {
    idsc.score /= num_records;
  }
  return avg;
}

void KMeansClustering::ConstructMatrix() {
  const int num_records = index_.NumRecords();
  const int num_keywords = index_.NumKeywords();
  record_matrix_.resize(num_records);
  for (int k = 0; k < num_keywords; ++k) {
    const Index::Keyword& keyword = index_.KeywordById(k);
    for (const Index::Item& item: keyword.items) {
      record_matrix_[item.record_id].push_back({k, item.score});
    }
  }
}

auto KMeansClustering::RandomCentroids(const size_t k, const size_t m) const
    -> vector<vector<IdScore> > {
  vector<vector<IdScore> > centroids;
  centroids.reserve(k);
  for (size_t i = 0; i < k; ++i) {
    centroids.push_back(record_matrix_[i]);
    // centroids.push_back({});
    // centroids.back().reserve(m);
    // for (size_t j = 0; j < m; ++j) {
    //   centroids.back().push_back({j, j});
    // }
  }
  return centroids;
}

void KMeansClustering::ComputeClustering(const size_t k, const size_t m,
                                         const float min_roc) {
  for (vector<IdScore>& vec: record_matrix_) {
    Truncate(m, &vec);
    Normalize(&vec);
  }
  vector<vector<IdScore> > centroids = RandomCentroids(k, m);
  for (vector<IdScore>& vec: centroids) {
    Truncate(m, &vec);
    Normalize(&vec);
  }
  vector<vector<int> > clusters(k);
  auto Rss = [this, &centroids, &clusters]() -> float {
    float rss = 0.0f;
    for (size_t end = clusters.size(), c = 0; c < end; ++c) {
      for (const int record: clusters[c]) {
        rss += Distance(centroids[c], this->RecordVector(record));
      }
    }
    return rss;
  };

  size_t num_iterations = 10000;
  float prev_rss = 9999999999.0f;
  while (num_iterations--) {
    for (size_t end = record_matrix_.size(), r = 0; r < end; ++r) {
      int best_id = 0;
      float best_dist = 1.1f;
      for (size_t c = 0; c < k; ++c) {
        float dist = Distance(record_matrix_[r], centroids[c]);
        if (dist < best_dist) {
          best_dist = dist;
          best_id = c;
        }
      }
      clusters[best_id].push_back(r);
    }
    for (size_t c = 0; c < k; ++c) {
      centroids[c] = Average(clusters[c]);
    }
    const float rss = Rss();
    if (prev_rss - rss < min_roc) {
      break;
    }
    std::cout << "\r" << rss << "                    " << std::flush;
    prev_rss = rss;
  }
  std::cout << std::endl;
}

auto KMeansClustering::RecordVector(const int record_id) const
    -> const vector<IdScore>& {
  assert(record_id > -1 && record_id < static_cast<int>(record_matrix_.size()));
  return record_matrix_[record_id];
}
