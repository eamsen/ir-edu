// Copyright 2013 Eugen Sawin <esawin@me73.com>
#include "./k-means-clustering.h"
#include <cassert>
#include <algorithm>
#include "./index.h"

using std::vector;

KMeansClustering::KMeansClustering(const Index& index)
    : index_(index) {}

void KMeansClustering::Truncate(size_t m, vector<IdScore>* vec) {
  m = std::min(m, vec->size());
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

void KMeansClustering::Normalize(vector<IdScore>* vec) {
  const float norm = Norm(*vec);
  if (norm == 0.0f) {
    return;
  }
  for (IdScore& idsc: *vec) {
    idsc.score /= norm;
  }
}

float KMeansClustering::Distance(const int r1, const int r2) const {
  const auto& r1_vec = RecordVector(r1);
  auto r1_it = r1_vec.begin();
  const auto r1_end = r1_vec.cend();
  const auto& r2_vec = RecordVector(r2);
  auto r2_it = r2_vec.begin();
  const auto r2_end = r2_vec.cend();
  float dist = 1;
  while (r1_it != r1_end && r2_it != r2_end) {
    while (r1_it != r1_end && *r1_it < *r2_it) {
      ++r1_it;
    }
    if (r1_it == r1_end) {
      break;
    }
    while (r2_it != r2_end && *r2_it < *r1_it) {
      ++r2_it;
    }
    while (r1_it != r1_end && r2_it != r2_end && *r1_it == *r2_it) {
      dist -= r1_it->score * r2_it->score;
      ++r1_it;
      ++r2_it;
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

auto KMeansClustering::RecordVector(const int record_id) const
    -> const vector<IdScore>& {
  assert(record_id > -1 && record_id < static_cast<int>(record_matrix_.size()));
  return record_matrix_[record_id];
}
