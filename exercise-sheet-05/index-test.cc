// Copyright 2012 Eugen Sawin <esawin@me73.com>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>
#include <vector>
#include <set>
#include "./index.h"

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
    urls_.push_back("Nikola_Tesla1");
    sentences_ += urls_.back() + "\tLegacy and honors The tesla (symbol T) – " +
                  "compound derived SI unit of magnetic flux density.\n";
    urls_.push_back("Nikola_Tesla2");
    sentences_ += urls_.back() + "\tLegacy and honors Google honoured Tesla " +
                  "on his birthday on 10 July 2009 by displaying a doodle in " +
                  "the Google search home page, that showed the G as a tesla " +
                  "coil.\n";
    urls_.push_back("Benjamin_Lamme");
    sentences_ += urls_.back() + "\tAfter Nikola Tesla left Westinghouse, " +
                  "Lamme redesigned the induction motor, making it as we " +
                  "know it today.\n";
    urls_.push_back("Thomas_Edison1");
    sentences_ += urls_.back() + "\tTesla once said that if Edison had to " +
                  "find a needle in a haystack he would take apart the " +
                  "haystack one straw at a time.\n";
    urls_.push_back("Thomas_Edison2");
    sentences_ += urls_.back() + "\tThe details of what happened are not " +
                  "known but Tesla who had once worked for Edison quit when " +
                  "he was promised a large bonus for solving a problem and " +
                  "then after being successful was told the promise was a " +
                  "joke.\n";
    urls_.push_back("Weird");
    sentences_ += urls_.back() + "\tME73 37signals\n";
    urls_.push_back("Weird");
    sentences_ += urls_.back() + "\tRoogla!44 mac\n";

    Index::AddRecordsFromCsv(sentences_, &index_);
    ASSERT_EQ(urls_.size() - 1, index_.NumRecords());
  }

  void TearDown() {
  }

  vector<string> urls_;
  string sentences_;
  Index index_;
};

TEST_F(IndexTest, Records) {
  ASSERT_EQ(urls_.size() - 1, index_.NumRecords());
  for (size_t i = 0; i < urls_.size() - 1; ++i) {
    EXPECT_EQ(urls_[i], index_.RecordById(i).url);
  }
}

TEST_F(IndexTest, noItems) {
  {
    const vector<Index::Item>& items = index_.Items("Nebuchad");
    EXPECT_EQ(0, items.size());
  }
  {
    const vector<Index::Item>& items = index_.Items("10");
    EXPECT_EQ(0, items.size());
  }
  {
    const vector<Index::Item>& items = index_.Items("2009");
    EXPECT_EQ(0, items.size());
  }
}

TEST_F(IndexTest, weirdItems) {
  {
    const vector<Index::Item>& items = index_.Items("ME73");
    EXPECT_EQ(1, items.size());
  }
  {
    const vector<Index::Item>& items = index_.Items("37signals");
    EXPECT_EQ(1, items.size());
  }
  {
    const vector<Index::Item>& items = index_.Items("mac");
    EXPECT_EQ(1, items.size());
    EXPECT_EQ(vector<Index::Item>({ {6, {25}, 3, 0.0f} }),
              items);
  }
}

TEST_F(IndexTest, teslaItems) {
  const vector<Index::Item>& items = index_.Items("tesla");
  ASSERT_EQ(6, items.size());
  EXPECT_EQ(vector<Index::Item>({ {0, {98}, 5, 0.0f}, {1, {22}, 5, 0.0f},
                                  {2, {34, 150}, 5, 0.0f}, {3, {13}, 5, 0.0f},
                                  {4, {0}, 5, 0.0f}, {5, {47}, 5, 0.0f} }),
            items);
  const vector<Index::Item>& items2 = index_.Items("Tesla");
  EXPECT_EQ(items, items2);
}

TEST_F(IndexTest, atomsItems) {
  const vector<Index::Item>& items = index_.Items("atoms");
  ASSERT_EQ(1, items.size());
  EXPECT_EQ(vector<Index::Item>({ {0, {65}, 5, 0.0f} }),
            items);
}

TEST_F(IndexTest, EdisonItems) {
  const vector<Index::Item>& items = index_.Items("Edison");
  ASSERT_EQ(2, items.size());
  EXPECT_EQ(vector<Index::Item>({ {4, {24}, 6, 0.0f}, {5, {77}, 6, 0.0f} }),
            items);
}

TEST_F(IndexTest, GoogleItems) {
  const vector<Index::Item>& items = index_.Items("Google");
  ASSERT_EQ(1, items.size());
  EXPECT_EQ(vector<Index::Item>({ {2, {18, 102}, 6, 0.0f} }),
            items);
}

TEST_F(IndexTest, NGrams) {
  EXPECT_EQ(vector<string>({}),
            Index::NGrams("", 2));
  EXPECT_EQ(vector<string>({}),
            Index::NGrams("", 3));
  EXPECT_EQ(vector<string>({}),
            Index::NGrams("h", 3));
  EXPECT_EQ(vector<string>({"#h", "h#"}),
            Index::NGrams("h", 2));
  EXPECT_EQ(vector<string>({"#ha", "ha#"}),
            Index::NGrams("ha", 3));
  EXPECT_EQ(vector<string>({"#ha", "hal", "al#"}),
            Index::NGrams("hal", 3));
  EXPECT_EQ(vector<string>({"#ha", "hal", "all", "llo", "lo#"}),
            Index::NGrams("hallo", 3));
  EXPECT_EQ(vector<string>({"#hal", "hall", "allo", "llo#"}),
            Index::NGrams("hallo", 4));
  EXPECT_EQ(vector<string>({"#hall", "hallo", "allo#"}),
            Index::NGrams("hallo", 5));
  EXPECT_EQ(vector<string>({"#in", "inf", "nfo", "for", "orm", "rma", "mat",
                            "ati", "tik", "ik#"}),
            Index::NGrams("informatik", 3));
  EXPECT_EQ(vector<string>({"#inf", "info", "nfor", "form", "orma", "rmat",
                            "mati", "atik", "tik#"}),
            Index::NGrams("informatik", 4));
  EXPECT_EQ(vector<string>({"#info", "infor", "nform", "forma", "ormat",
                            "rmati", "matik", "atik#"}),
            Index::NGrams("informatik", 5));
  EXPECT_EQ(vector<string>({"#infor", "inform", "nforma", "format", "ormati",
                            "rmatik", "matik#"}),
            Index::NGrams("informatik", 6));
  EXPECT_EQ(vector<string>({"#inform", "informa", "nformat", "formati",
                            "ormatik", "rmatik#"}),
            Index::NGrams("informatik", 7));
  EXPECT_EQ(vector<string>({"#informa", "informat", "nformati", "formatik",
                            "ormatik#"}),
            Index::NGrams("informatik", 8));
  EXPECT_EQ(vector<string>({"#informat", "informati", "nformatik",
                            "formatik#"}),
            Index::NGrams("informatik", 9));
  EXPECT_EQ(vector<string>({"#informati", "informatik", "nformatik#"}),
            Index::NGrams("informatik", 10));
  EXPECT_EQ(vector<string>({"#informatik", "informatik#"}),
            Index::NGrams("informatik", 11));
  EXPECT_EQ(vector<string>({}),
            Index::NGrams("informatik", 12));
}

TEST_F(IndexTest, EditDistance) {
  EXPECT_EQ(0, Index::EditDistance("", ""));
  EXPECT_EQ(0, Index::EditDistance("board", "board"));
  EXPECT_EQ(1, Index::EditDistance("a", ""));
  EXPECT_EQ(1, Index::EditDistance("", "b"));
  EXPECT_EQ(4, Index::EditDistance("", "abba"));
  EXPECT_EQ(4, Index::EditDistance("abba", ""));
  EXPECT_EQ(1, Index::EditDistance("abba", "aba"));
  EXPECT_EQ(2, Index::EditDistance("abba", "aa"));
  EXPECT_EQ(2, Index::EditDistance("aba", "arra"));
  EXPECT_EQ(3, Index::EditDistance("board", "bread"));
  EXPECT_EQ(3, Index::EditDistance("cats", "fast"));
  EXPECT_EQ(3, Index::EditDistance("spartan", "part"));
  EXPECT_EQ(4, Index::EditDistance("zeil", "trials"));
}

TEST_F(IndexTest, Union) {
  auto StlUnion = [](const vector<vector<int> >& lists) {
    set<int> list_union;
    for (const vector<int>& list: lists) {
      list_union.insert(list.begin(), list.end());
    }
    return vector<int>(list_union.begin(), list_union.end());
  };

  vector<int> v0;
  vector<int> v1 = {1};
  vector<int> v2 = {2};
  vector<int> v3 = {3};
  vector<int> v4 = {1, 2, 3};
  vector<int> v5 = {1, 2};
  vector<int> v6 = {1, 3};
  vector<int> v7 = {2, 3};
  vector<int> v8 = {5};
  vector<int> v9 = {4, 6};
  vector<int> freqs;

  EXPECT_EQ(v0, Index::Union({}));
  EXPECT_EQ(v0, Index::Union({&v0}));
  EXPECT_EQ(v0, Index::Union({&v0, &v0}, &freqs));
  EXPECT_EQ(vector<int>({}), freqs);
  EXPECT_EQ(v1, Index::Union({&v1}));
  EXPECT_EQ(v1, Index::Union({&v1, &v1}, &freqs));
  EXPECT_EQ(vector<int>({2}), freqs);
  EXPECT_EQ(v1, Index::Union({&v1, &v1, &v1}));
  EXPECT_EQ(v4, Index::Union({&v4}));
  EXPECT_EQ(v4, Index::Union({&v4, &v4}));
  EXPECT_EQ(v4, Index::Union({&v4, &v4, &v4}, &freqs));
  EXPECT_EQ(vector<int>({3, 3, 3}), freqs);

  EXPECT_EQ(StlUnion({v0, v1}), Index::Union({&v0, &v1}, &freqs));
  EXPECT_EQ(vector<int>({1}), freqs);
  EXPECT_EQ(StlUnion({v0, v2}), Index::Union({&v0, &v2}));
  EXPECT_EQ(StlUnion({v0, v3}), Index::Union({&v0, &v3}));
  EXPECT_EQ(StlUnion({v0, v4}), Index::Union({&v0, &v4}));
  EXPECT_EQ(StlUnion({v0, v5}), Index::Union({&v0, &v5}));
  EXPECT_EQ(StlUnion({v0, v6}), Index::Union({&v0, &v6}));
  EXPECT_EQ(StlUnion({v0, v7}), Index::Union({&v0, &v7}));
  EXPECT_EQ(StlUnion({v0, v8}), Index::Union({&v0, &v8}));
  EXPECT_EQ(StlUnion({v0, v9}), Index::Union({&v0, &v9}));

  EXPECT_EQ(StlUnion({v1, v2}), Index::Union({&v1, &v2}));
  EXPECT_EQ(StlUnion({v1, v3}), Index::Union({&v1, &v3}));
  EXPECT_EQ(StlUnion({v1, v4}), Index::Union({&v1, &v4}, &freqs));
  EXPECT_EQ(vector<int>({2, 1, 1}), freqs);
  EXPECT_EQ(StlUnion({v1, v5}), Index::Union({&v1, &v5}, &freqs));
  EXPECT_EQ(vector<int>({2, 1}), freqs);
  EXPECT_EQ(StlUnion({v1, v6}), Index::Union({&v1, &v6}));
  EXPECT_EQ(StlUnion({v1, v7}), Index::Union({&v1, &v7}));
  EXPECT_EQ(StlUnion({v1, v8}), Index::Union({&v1, &v8}));
  EXPECT_EQ(StlUnion({v1, v9}), Index::Union({&v1, &v9}));

  EXPECT_EQ(StlUnion({v2, v3}), Index::Union({&v2, &v3}, &freqs));
  EXPECT_EQ(vector<int>({1, 1}), freqs);
  EXPECT_EQ(StlUnion({v2, v4}), Index::Union({&v2, &v4}, &freqs));
  EXPECT_EQ(vector<int>({1, 2, 1}), freqs);
  EXPECT_EQ(StlUnion({v2, v5}), Index::Union({&v2, &v5}));
  EXPECT_EQ(StlUnion({v2, v6}), Index::Union({&v2, &v6}));
  EXPECT_EQ(StlUnion({v2, v7}), Index::Union({&v2, &v7}));
  EXPECT_EQ(StlUnion({v2, v8}), Index::Union({&v2, &v8}));
  EXPECT_EQ(StlUnion({v2, v9}), Index::Union({&v2, &v9}));

  EXPECT_EQ(StlUnion({v3, v4}), Index::Union({&v3, &v4}));
  EXPECT_EQ(StlUnion({v3, v5}), Index::Union({&v3, &v5}));
  EXPECT_EQ(StlUnion({v3, v6}), Index::Union({&v3, &v6}));
  EXPECT_EQ(StlUnion({v3, v7}), Index::Union({&v3, &v7}));
  EXPECT_EQ(StlUnion({v3, v8}), Index::Union({&v3, &v8}));
  EXPECT_EQ(StlUnion({v3, v9}), Index::Union({&v3, &v9}));

  EXPECT_EQ(StlUnion({v4, v5}), Index::Union({&v4, &v5}, &freqs));
  EXPECT_EQ(vector<int>({2, 2, 1}), freqs);
  EXPECT_EQ(StlUnion({v4, v6}), Index::Union({&v4, &v6}, &freqs));
  EXPECT_EQ(vector<int>({2, 1, 2}), freqs);
  EXPECT_EQ(StlUnion({v4, v7}), Index::Union({&v4, &v7}));
  EXPECT_EQ(StlUnion({v4, v8}), Index::Union({&v4, &v8}));
  EXPECT_EQ(StlUnion({v4, v9}), Index::Union({&v4, &v9}));

  EXPECT_EQ(StlUnion({v5, v6}), Index::Union({&v5, &v6}));
  EXPECT_EQ(StlUnion({v5, v7}), Index::Union({&v5, &v7}));
  EXPECT_EQ(StlUnion({v5, v8}), Index::Union({&v5, &v8}));
  EXPECT_EQ(StlUnion({v5, v9}), Index::Union({&v5, &v9}));

  EXPECT_EQ(StlUnion({v6, v7}), Index::Union({&v6, &v7}));
  EXPECT_EQ(StlUnion({v6, v8}), Index::Union({&v6, &v8}));
  EXPECT_EQ(StlUnion({v6, v9}), Index::Union({&v6, &v9}));

  EXPECT_EQ(StlUnion({v7, v8}), Index::Union({&v7, &v8}));
  EXPECT_EQ(StlUnion({v7, v9}), Index::Union({&v7, &v9}));

  EXPECT_EQ(StlUnion({v8, v9}), Index::Union({&v8, &v9}));

  EXPECT_EQ(StlUnion({v0, v1, v2}), Index::Union({&v0, &v1, &v2}));
  EXPECT_EQ(StlUnion({v0, v1, v2, v3}), Index::Union({&v0, &v1, &v2, &v3}));
  EXPECT_EQ(StlUnion({v0, v1, v2, v3, v4}),
            Index::Union({&v0, &v1, &v2, &v3, &v4}));
  EXPECT_EQ(StlUnion({v0, v1, v2, v3, v4, v5}),
            Index::Union({&v0, &v1, &v2, &v3, &v4, &v5}));
  EXPECT_EQ(StlUnion({v0, v1, v2, v3, v4, v5, v6}),
            Index::Union({&v0, &v1, &v2, &v3, &v4, &v5, &v6}));
  EXPECT_EQ(StlUnion({v0, v1, v2, v3, v4, v5, v6, v7}),
            Index::Union({&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7}));
  EXPECT_EQ(StlUnion({v0, v1, v2, v3, v4, v5, v6, v7, v8}),
            Index::Union({&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8}));
  EXPECT_EQ(StlUnion({v0, v1, v2, v3, v4, v5, v6, v7, v8, v9}),
            Index::Union({&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8, &v9},
                         &freqs));
  EXPECT_EQ(vector<int>({4, 4, 4, 1, 1, 1}), freqs);
  EXPECT_EQ(StlUnion({v4, v1, v6, v9, v7, v8, v9}),
            Index::Union({&v4, &v1, &v6, &v9, &v7, &v8, &v9}));
}
