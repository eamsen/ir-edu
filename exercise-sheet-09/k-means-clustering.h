// Copyright 2013 Eugen Sawin <esawin@me73.com>
#ifndef EXERCISE_SHEET_09_K_MEANS_CLUSTERING_H_
#define EXERCISE_SHEET_09_K_MEANS_CLUSTERING_H_

#include <vector>
#include <string>

class Index;

// K-means clustering based on given index.
class KMeansClustering {
 public:
  // Stores a document id and score; used for sparse vector representation.
  struct IdScore {
    int id;
    float score;
    // Used for sorting.
    bool operator<(const IdScore& rhs) const {
      return id < rhs.id;
    }
    bool operator==(const IdScore& rhs) const {
      return id == rhs.id && score == rhs.score;
    }
  };

  // Invalid id value.
  static const int kInvalidId;

  // Truncates the given vector to m dimensions with he highest scores.
  static void Truncate(size_t m, std::vector<IdScore>* vec);

  // Normalizes the given vector.
  static void Normalize(std::vector<IdScore>* vec);

  // Returns the distance measure between given two normalized record vectors.
  static float Distance(const std::vector<IdScore>& v1,
                        const std::vector<IdScore>& v2);

  // Initializes the clustering with given index.
  explicit KMeansClustering(const Index& index);

  // Returns the vector composed by the average of the given records.
  std::vector<IdScore> Average(const std::vector<int>& records) const;

  // Constructs the record-term matrix.
  void ConstructMatrix();

  // Computes the k-means clustering for given numer of clusters k, maximum
  // vector dimensions m. Terminates when dropping below the given minimum rate
  // of change or reaching the given maximum number of iterations.
  // Returns the final residual sum of squares value.
  float ComputeClustering(const size_t k, const size_t m, const float min_roc,
                          const size_t max_num_iter);

  // Returns the terms vector for given record id;
  const std::vector<IdScore>& RecordVector(const int record_id) const;

  // Returns the terms vector for given centroid id.
  const std::vector<IdScore>& Centroid(const int id) const;

  // Returns the record ids for given cluster id.
  const std::vector<int>& Cluster(const int id) const;

  // Returns the number of iterations used during the last clustering.
  size_t LastNumIters() const;

 private:
  // Random centroids seeding.
  std::vector<std::vector<IdScore> > RandomCentroids(const size_t k) const;

  // K-means++ probability based seeding.
  std::vector<std::vector<IdScore> > PPCentroids(const size_t k) const;

  // Centroids seeding based on farthest vectors.
  std::vector<std::vector<IdScore> > FarthestCentroids(const size_t k) const;

  // Returns the record id of the farthest record vector for given centroids.
  int NextFarthestCentroid(
      const std::vector<std::vector<IdScore> >& centroids,
      std::vector<float>* dists = 0) const;

  // Updates the centroids based on the clustering assignments.
  void UpdateCentroids(const size_t k, const size_t m);

  // Assigns record vectors to the nearest centroid. Returns the RSS value.
  float UpdateClusters(const size_t k);

  const Index& index_;
  std::vector<std::vector<IdScore> > record_matrix_;
  std::vector<std::vector<IdScore> > centroids_;
  std::vector<std::vector<int> > clusters_;
  size_t num_iters_;
};

#endif  // EXERCISE_SHEET_09_K_MEANS_CLUSTERING_H_
