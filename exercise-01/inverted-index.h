// Copyright 2012 Eugen Sawin <esawin@me73.com>
#ifndef EXERCISE_01_INVERTED_INDEX_H_
#define EXERCISE_01_INVERTED_INDEX_H_

#include <unordered_map>
#include <string>
#include <vector>

// The inverted index holding a mapping from keywords (prefixes) to records.
class Index {
 public:
  struct Record {
    Record(const std::string& url, const std::string& content)
        : url(url), content(content) {}

    std::string url;
    std::string content;
  };

  struct PosSize {
    size_t pos;
    size_t size;
  };

  static const size_t kMinKeywordSize;
  static const int kInvalidId;

  static std::vector<PosSize>
    ExtractKeywords(const std::string& content, const size_t beg,
                    const size_t end);

  // Creates an inverted index from given csv file of records in the format
  // <url>\t<content>\n
  static void AddRecordsFromCsvFile(const std::string& file_name,
                                    Index* inverted_index);

  Index();

  // Adds the record to the index by processing its contents.
  // Returns the new record id.
  int AddRecord(const std::string& url, const std::string& content);

  // Adds the item with given keywrod and record id to the index.
  // Returns the new total number of items in the index.
  int AddItem(const std::string& keyword, const int record_id);

  int KeywordId(const std::string& keyword) const;

  // Write inverted lists lengths to standard output.
  void OutputInvertedListLengths() const;

 private:
  std::unordered_map<std::string, std::vector<int> > index_;
  std::vector<Record> records_;
  size_t num_items_;
};

#endif  // EXERCISE_01_INVERTED_INDEX_H_
