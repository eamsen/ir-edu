// Copyright 2013 Eugen Sawin <esawin@me73.com>
#ifndef EXERCISE_SHEET_09_K_MEANS_CLUSTERING_H_
#define EXERCISE_SHEET_09_K_MEANS_CLUSTERING_H_

class Index;

class KMeansClustering {
 public:
  explicit KMeansClustering(const Index& index);

 private:
  const Index& index_;
};

#endif  // EXERCISE_SHEET_09_K_MEANS_CLUSTERING_H_
