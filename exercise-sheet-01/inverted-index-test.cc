// Copyright 2012 Eugen Sawin <esawin@me73.com>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>
#include <vector>
#include <set>
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

class IndexTest : public ::testing::Test {
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
  }

  void TearDown() {
  }

  vector<string> urls_;
  string sentences_;
  Index index_;
};

TEST_F(IndexTest, Records) {
  ASSERT_EQ(urls_.size(), index_.NumRecords());
  for (size_t i = 0; i < urls_.size(); ++i) {
    EXPECT_EQ(urls_[i], index_.RecordById(i).url);
  }
}

TEST_F(IndexTest, teslaItems) {
  const vector<Index::Item>& items = index_.Items("tesla");
  ASSERT_EQ(3, items.size());
  EXPECT_EQ(vector<Index::Item>({ {0, 98, 5}, {1, 22, 5}, {2, 150, 5} }),
            items);
}

TEST_F(IndexTest, atomsItems) {
  const vector<Index::Item>& items = index_.Items("atoms");
  ASSERT_EQ(1, items.size());
  EXPECT_EQ(vector<Index::Item>({ {0, 65, 5} }),
            items);
}

TEST_F(IndexTest, TeslaItems) {
  const vector<Index::Item>& items = index_.Items("Tesla");
  ASSERT_EQ(4, items.size());
  EXPECT_EQ(vector<Index::Item>({ {2, 34, 5}, {3, 13, 5}, {4, 0, 5},
                                  {5, 47, 5} }),
            items);
}

TEST_F(IndexTest, EdisonItems) {
  const vector<Index::Item>& items = index_.Items("Edison");
  ASSERT_EQ(2, items.size());
  EXPECT_EQ(vector<Index::Item>({ {4, 24, 6}, {5, 77, 6} }),
            items);
}

TEST_F(IndexTest, GoogleItems) {
  const vector<Index::Item>& items = index_.Items("Google");
  ASSERT_EQ(2, items.size());
  EXPECT_EQ(vector<Index::Item>({ {2, 18, 6}, {2, 102, 6} }),
            items);
}
