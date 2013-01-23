// Copyright 2013 Eugen Sawin <esawin@me73.com>
#include <cassert>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <functional>
#include <algorithm>
#include <cmath>
#include "./index.h"
#include "./profiler.h"
#include "./clock.h"
#include "./k-means-clustering.h"

using std::string;

// Returns the file size of given file. Returns 0, if the file is not found.
size_t FileSize(const string& path) {
  using std::ifstream;

  ifstream stream(path.c_str());
  size_t size = 0;
  if (stream.good()) {
    stream.seekg(0, std::ios::end);
    size = stream.tellg();
    stream.seekg(0, std::ios::beg);
  }
  return size;
}

// Reads the whole file from given path and returns the content.
string ReadFile(const string& path) {
  using std::ifstream;

  const size_t file_size = FileSize(path);
  string content;
  content.resize(file_size);
  ifstream stream(path.c_str());
  stream.read(&content[0], file_size);
  return content;
}

// Main function.
int main(int argc, char** argv) {
  using std::cout;
  using std::cin;
  using std::endl;

  float bm25_b = 0.75f;
  float bm25_k = 1.75f;
  size_t k = 50;
  size_t m = 1000;
  size_t max_num_iter = 100;
  float min_roc = 0.1f;
  // Parse command line arguments.
  if (argc < 2) {
    cout << "Usage: exercise05-main <CSV-file> [max-num-iterations]" << endl;
    return 1;
  }
  if (argc > 2) {
    std::stringstream(argv[2]) >> max_num_iter;
  }
  Index index;
  auto start = Clock();
  Index::AddRecordsFromCsv(ReadFile(argv[1]), &index);
  index.ComputeScores(bm25_b, bm25_k);
  KMeansClustering cluster(index);
  cluster.ConstructMatrix();
  auto end = Clock();
  cout << "Number of records: " << index.NumRecords()
       << "\nNumber of items: " << index.NumItems()
       << "\nIndex construction time: " << end - start
       << "\nBM25 parameters: b = " << bm25_b << ", k = " << bm25_k
       << endl;
  start = Clock(Clock::kThreadCpuTime);
  Profiler::Start("clustering.prof");
  const float rss = cluster.ComputeClustering(k, m, min_roc, max_num_iter);
  Profiler::Stop();
  cout << "Number of iterations: " << cluster.LastNumIters() << endl;
  cout << "Final RSS: " << rss << endl;
  cout << "Clustering time: " << Clock(Clock::kThreadCpuTime) - start << endl;
  // Output the clusters.
  std::ofstream cluster_file("clusters.txt");
  for (size_t c = 0; c < k; ++c) {
    auto centroid = cluster.Centroid(c);
    KMeansClustering::Truncate(10, &centroid);
    for (auto end = centroid.end(), it = centroid.begin(); it != end; ++it) {
      if (it != centroid.begin()) {
        cluster_file << ", ";
      }
      cluster_file << index.KeywordById(it->id).name;
    }
    cluster_file << "\n";
  }
  return 0;
}
