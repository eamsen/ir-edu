// Copyright 2012 Eugen Sawin <esawin@me73.com>
#ifndef EXERCISE_SHEET_02_INVERTED_INDEX_H_
#define EXERCISE_SHEET_02_INVERTED_INDEX_H_

#include <unordered_map>
#include <string>
#include <vector>

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

  // An item depicts an occurence of a keyword within a record at some position.
  struct Item {
    bool operator==(const Item& rhs) const {
      return record_id == rhs.record_id && pos == rhs.pos && size == rhs.size;
    }

    int record_id;
    size_t pos;
    size_t size;
  };

  // The minimum size for a valid keyword.
  static const size_t kMinKeywordSize;

  // Invalid index value, used for record ids.
  static const int kInvalidId;

  // Finds all valid keywords within given content string and returns their
  // position and sizes. A keyword must contain at least one alphabetic
  // character and have at least the minimum size.
  static std::vector<PosSize>
    ExtractKeywords(const std::string& content, const size_t beg,
                    const size_t end);

  // Adds all records and items from given CSV content, if the file format is:
  // <url>\t<content>\n
  static void AddRecordsFromCsv(const std::string& file_content,
                                Index* inverted_index);

  // Adds all records and items from given CSV file, if the file format is:
  // <url>\t<content>\n
  static void AddRecordsFromCsvFile(const std::string& filename,
                                    Index* inverted_index);

  // Default index initialization.
  Index();

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
  int AddItem(const std::string& keyword, const int record_id,
              const size_t pos);

  // Reserves space for given number of records.
  void ReserveRecords(const size_t num);

  // Returns the number of records indexed.
  size_t NumRecords() const;

  // Returns the total number of items (keyword occurences) indexed.
  size_t NumItems() const;

  // Write inverted lists lengths to standard output.
  void OutputInvertedListLengths() const;

 private:
  std::unordered_map<std::string, std::vector<Item> > index_;
  std::vector<Record> records_;
  size_t num_items_;
};

#endif  // EXERCISE_SHEET_02_INVERTED_INDEX_H_
