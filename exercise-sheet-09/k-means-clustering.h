// Copyright 2013 Eugen Sawin <esawin@me73.com>
#ifndef EXERCISE_SHEET_09_K_MEANS_CLUSTERING_H_
#define EXERCISE_SHEET_09_K_MEANS_CLUSTERING_H_

#include <vector>
#include <string>

class Index;

class KMeansClustering {
 public:
  struct IdScore {
    int id;
    float score;
    // Used for intersection.
    bool operator<(const IdScore& rhs) const {
      return id < rhs.id;
    }
    bool operator==(const IdScore& rhs) const {
      return id == rhs.id;
    }
  };
  static const int kInvalidId;
  static void Truncate(size_t m, std::vector<IdScore>* vec);
  static void Normalize(std::vector<IdScore>* vec);

  explicit KMeansClustering(const Index& index);
  float Distance(const std::vector<IdScore>& v1,
                 const std::vector<IdScore>& v2) const;
  std::vector<IdScore> Average(const std::vector<int>& records) const;
  void ConstructMatrix();
  void ComputeClustering(const size_t k, const size_t m, const float min_roc,
                         const size_t max_num_iter);
  std::vector<std::vector<IdScore> > FarthestCentroids(const size_t k) const;
  std::vector<std::vector<IdScore> > RandomCentroids(const size_t k) const;
  const std::vector<IdScore>& RecordVector(const int record_id) const;

 private:
  int NextFarthestCentroid(const std::vector<std::vector<IdScore> >& centroids)
      const;

  const Index& index_;
  std::vector<std::vector<IdScore> > record_matrix_;
};

#endif  // EXERCISE_SHEET_09_K_MEANS_CLUSTERING_H_
