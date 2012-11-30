// Copyright 2012 Eugen Sawin <esawin@me73.com>
#ifndef EXERCISE_SHEET_05_INDEX_H_
#define EXERCISE_SHEET_05_INDEX_H_

#include <unordered_map>
#include <string>
#include <vector>
#include "./clock.h"

// The inverted index holding a mapping from keywords (prefixes) to records.
class Index {
 public:
  // A record consists of its url and the content text.
  struct Record {
    std::string url;
    std::string content;
  };

  // Intermediate keyword position structure used during extraction.
  struct PosSize {
    size_t pos;
    size_t size;
  };

  // An item depicts the occurences of a keyword within a record.
  struct Item {
    Item(const int record_id, const std::vector<size_t>& pos, const size_t size,
         const float score)
        : record_id(record_id),
          size(size),
          score(score),
          positions(pos) {}

    bool operator==(const Item& rhs) const {
      return record_id == rhs.record_id && positions == rhs.positions;
    }

    int record_id;
    size_t size;
    float score;
    std::vector<size_t> positions;
  };

  // A keyword consists of its name and its items, i.e. occurrences in records.
  struct Keyword {
    explicit Keyword(const std::string& name) : name(name) {}
    operator const std::string&() const {
      return name;
    }
    std::string name;
    std::vector<Item> items;
  };

  // The minimum size for a valid keyword.
  static const size_t kMinKeywordSize;

  // Invalid index value, used for record ids.
  static const int kInvalidId;

  // All whitespace characters, useful as default delimeter for splitting.
  static const char* kWhitespace;

  // Splits the content at any of given delimeters.
  static std::vector<std::string> Split(const std::string& content,
                                        const std::string& delims);

  // Finds all valid keywords within given content string and returns their
  // position and sizes. A keyword must contain at least one alphabetic
  // character and have at least the minimum size.
  static std::vector<PosSize>
    ExtractKeywords(const std::string& content, const size_t beg,
                    const size_t end);

  // Adds all records and items from given CSV content, if the file format is:
  // <url>\t<content>\n
  static void AddRecordsFromCsv(const std::string& file_content, Index* index);

  // Adds all keywords from given content, if the file format is:
  // <keyword>\n
  static void AddKeywords(const std::string& file_content, Index* index);

  // Creates all n-grams for given word and n value.
  static std::vector<std::string> NGrams(const std::string& word,
                                         const int ngram_n);

  // Creates all n-grams for given words and n value. The word parts are
  // meant to be parts of a query separated at wildcards in conserved order.
  static std::vector<std::string> NGrams(const std::vector<std::string>& words,
                                         const int ngram_n);

  // Returns the edit distance between the given words.
  static int EditDistance(const std::string& word1, const std::string& word2);

  // Returns the union of the given lists.
  static std::vector<int> Union(
      const std::vector<const std::vector<int>*>& lists);

  // Returns the union of the given lists and writes the frequency of each
  // value in the provided output vector.
  static std::vector<int> Union(
      const std::vector<const std::vector<int>*>& lists,
      std::vector<int>* freqs);

  // Default index initialization.
  Index();

  // Returns all keyword, which are within the given edit
  // distance from the given keyword.
  std::vector<std::string> ApproximateMatches(const std::string& keyword,
                                              const int max_ed) const;

  // Computes BM25 scores, replacing the term frequency based defaults.
  void ComputeScores(const float bm25_b, const float bm25_k);

  // Builds the n-gram index with given parameter.
  void BuildNGrams(const int ngram_n);

  // Returns a const reference to the record of given id.
  const Record& RecordById(const int record_id) const;

  // Returns a const reference to the items list for given keyword.
  const std::vector<Item>& Items(const std::string& keyword) const;

  // Adds the record to the index.
  // Returns the new record id.
  int AddRecord(const std::string& url, const std::string& content);

  // Extends the content of a record with given id.
  // Returns the old size of the record content.
  size_t ExtendRecord(const int record_id, const std::string& content);

  // Adds the item with given keyword, record id and its position within the
  // record to the index.
  // Returns the new total number of items in the index.
  int AddItem(const int keyword_id, const int record_id,
              const size_t pos);

  // Adds the given n-gram to keyword mapping to the n-gram index.
  void AddNGram(const int keyword_id, const std::string& ngram);

  // Returns aht keyword ids for given n-gram.
  const std::vector<int>& NGramItems(const std::string& ngram) const;

  int KeywordId(const std::string& keyword) const;
  const Keyword& KeywordById(const int id) const;

  // Adds the keyword and creates all its n-grams.
  // Returns the id for the inserted keyword.
  int AddKeyword(const std::string& keyword);

  // Reserves space for given number of records.
  void ReserveRecords(const size_t num);

  // Returns the total size (in char) of all indexed records.
  size_t TotalSize() const;

  // Returns the number of records indexed.
  size_t NumRecords() const;

  // Returns the total number of items (keyword occurences) indexed.
  size_t NumItems() const;

  // Returns the number of keywords indexed.
  size_t NumKeywords() const;

  // Returns the average duration of all the edit distance computations during
  // the last call to ApproximateMatches in microseconds.
  Clock::Diff LastEdAvgDuration() const;

 private:
  // Returns a reference to the record of given id.
  Record& recordById(const int record_id);
  Keyword& keywordById(const int id);

  std::vector<Record> records_;
  std::unordered_map<std::string, int> keyword_index_;
  std::unordered_map<std::string, std::vector<int> > ngram_index_;
  std::vector<Keyword> keywords_;
  size_t num_items_;
  size_t total_size_;
  int ngram_n_;
  mutable Clock::Diff last_ed_avg_duration_;
};

#endif  // EXERCISE_SHEET_05_INDEX_H_
