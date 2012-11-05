// Copyright 2012 Eugen Sawin <esawin@me73.com>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>
#include <vector>
#include <set>
#include "./query-processor.h"
#include "./inverted-index.h"

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

class QueryProcessorTest : public ::testing::Test {
 public:
  void SetUp() {
    urls_.push_back("Jook_Walraven");
    sentences_ += urls_.back() + "\tTo keep the atomic hydrogen from forming " +
                  "hydrogen molecules, the atoms were spin-polarized by a 7 " +
                  "tesla magnet.\n";
    urls_.push_back("Nikola_Tesla");
    sentences_ += urls_.back() + "\tLegacy and honors The tesla (symbol T) – " +
                  "compound derived SI unit of magnetic flux density.\n";
    urls_.push_back("Nikola_Tesla");
    sentences_ += urls_.back() + "\tLegacy and honors Google honoured Tesla " +
                  "on his birthday on 10 July 2009 by displaying a doodle in " +
                  "the Google search home page, that showed the G as a tesla " +
                  "coil.\n";
    urls_.push_back("Benjamin_Lamme");
    sentences_ += urls_.back() + "\tAfter Nikola Tesla left Westinghouse, " +
                  "Lamme redesigned the induction motor, making it as we " +
                  "know it today.\n";
    urls_.push_back("Thomas_Edison");
    sentences_ += urls_.back() + "\tTesla once said that if Edison had to " +
                  "find a needle in a haystack he would take apart the " +
                  "haystack one straw at a time.\n";
    urls_.push_back("Thomas_Edison");
    sentences_ += urls_.back() + "\tThe details of what happened are not " +
                  "known but Tesla who had once worked for Edison quit when " +
                  "he was promised a large bonus for solving a problem and " +
                  "then after being successful was told the promise was a " +
                  "joke.\n";

    Index::AddRecordsFromCsv(sentences_, &index_);
    ASSERT_EQ(urls_.size(), index_.NumRecords());
    num_results_ = 0;
  }

  void TearDown() {
  }

  vector<string> urls_;
  string sentences_;
  Index index_;
  int num_results_;
};

TEST_F(QueryProcessorTest, emptyAnswer) {
  QueryProcessor proc(index_);
  {
    vector<Index::Item> results = proc.Answer("Nebuchad", num_results_);
    ASSERT_EQ(0, results.size());
  }
  {
    vector<Index::Item> results = proc.Answer("atoms motor", num_results_);
    ASSERT_EQ(0, results.size());
  }
}

TEST_F(QueryProcessorTest, dirtyQuery) {
  QueryProcessor proc(index_);
  {
    vector<Index::Item> results = proc.Answer("", num_results_);
    ASSERT_EQ(0, results.size());
  }
  {
    vector<Index::Item> results = proc.Answer("Tesla 33 Edison", num_results_);
    ASSERT_EQ(4, results.size());
    EXPECT_EQ(vector<Index::Item>({ {4, 0, 5}, {4, 24, 6},
                                    {5, 47, 5}, {5, 77, 6} }),
            results);
  }
  {
    vector<Index::Item> results = proc.Answer("hmm? Tesla  \ta Edison",
        num_results_);
    ASSERT_EQ(4, results.size());
    EXPECT_EQ(vector<Index::Item>({ {4, 0, 5}, {4, 24, 6},
                                    {5, 47, 5}, {5, 77, 6} }),
            results);
  }
}

TEST_F(QueryProcessorTest, teslaAnswer) {
  QueryProcessor proc(index_);
  vector<Index::Item> results = proc.Answer("tesla", num_results_);
  ASSERT_EQ(7, results.size());
  EXPECT_EQ(vector<Index::Item>({ {0, 98, 5}, {1, 22, 5}, {2, 34, 5},
                                  {2, 150, 5}, {3, 13, 5}, {4, 0, 5},
                                  {5, 47, 5} }),
            results);
  vector<Index::Item> results2 = proc.Answer("Tesla", num_results_);
  EXPECT_EQ(results, results2);
}

TEST_F(QueryProcessorTest, atomsAnswer) {
  QueryProcessor proc(index_);
  vector<Index::Item> results = proc.Answer("atoms", num_results_);
  ASSERT_EQ(1, results.size());
  EXPECT_EQ(vector<Index::Item>({ {0, 65, 5} }),
            results);
}

TEST_F(QueryProcessorTest, tesla_atomsAnswer) {
  QueryProcessor proc(index_);
  vector<Index::Item> results = proc.Answer("tesla atoms", num_results_);
  // ASSERT_EQ(2u, results.size());
  EXPECT_EQ(vector<Index::Item>({ {0, 98, 5}, {0, 65, 5} }),
            results);
}

TEST_F(QueryProcessorTest, EdisonAnswer) {
  QueryProcessor proc(index_);
  vector<Index::Item> results = proc.Answer("Edison", num_results_);
  ASSERT_EQ(2, results.size());
  EXPECT_EQ(vector<Index::Item>({ {4, 24, 6}, {5, 77, 6} }),
            results);
}

TEST_F(QueryProcessorTest, Tesla_EdisonAnswer) {
  QueryProcessor proc(index_);
  vector<Index::Item> results = proc.Answer("Tesla Edison", num_results_);
  ASSERT_EQ(4, results.size());
  EXPECT_EQ(vector<Index::Item>({ {4, 0, 5}, {4, 24, 6},
                                  {5, 47, 5}, {5, 77, 6} }),
            results);
}

TEST_F(QueryProcessorTest, GoogleAnswer) {
  QueryProcessor proc(index_);
  vector<Index::Item> results = proc.Answer("Google", num_results_);
  ASSERT_EQ(2, results.size());
  EXPECT_EQ(vector<Index::Item>({ {2, 18, 6}, {2, 102, 6} }),
            results);
}

TEST_F(QueryProcessorTest, longQuery) {
  QueryProcessor proc(index_);
  string query = "Google birthday doodle Tesla Legacy";
  vector<Index::Item> results = proc.Answer(query, num_results_);
  ASSERT_EQ(7, results.size());
  EXPECT_EQ(vector<Index::Item>({ {2, 18, 6}, {2, 102, 6},
                                  {2, 47, 8}, {2, 88, 6},
                                  {2, 34, 5}, {2, 150, 5},
                                  {2, 0, 6} }),
            results);
}
