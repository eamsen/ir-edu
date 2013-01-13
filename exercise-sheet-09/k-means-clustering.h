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

  static void Truncate(size_t m, std::vector<IdScore>* vec);
  static void Normalize(std::vector<IdScore>* vec);

  explicit KMeansClustering(const Index& index);
  float Distance(const int r1, const int r2) const;
  std::vector<IdScore> Average(const std::vector<int>& records) const;
  void ConstructMatrix();
  const std::vector<IdScore>& RecordVector(const int record_id) const;

 private:

  const Index& index_;
  std::vector<std::vector<IdScore> > record_matrix_;
};

#endif  // EXERCISE_SHEET_09_K_MEANS_CLUSTERING_H_
