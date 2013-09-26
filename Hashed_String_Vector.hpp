#ifndef HASHED_STRING_VECTOR_HPP
#define HASHED_STRING_VECTOR_HPP

#include <utility>
#include <string>
#include <vector>
#include <unordered_map>

class Hashed_String_Vector;

// maintain a bidirectional map between hashed_string_vector labels and integer codes
// assignment works as expected; retrieval returns a negative index, for non-existent
// string index, or a null string pointer, for non-existent integer index

class Hashed_String_Vector {

  // wasteful, as we maintain two copies of each string,
  // one for the hash and one for the vector.

private:
  int count;
  std::vector < std::string > strings;
  typedef std::unordered_map < std::string, int > mymap;
  mymap indexes;

public:

  Hashed_String_Vector() :
    count(0),
    strings(),
    indexes()
  {
  };

  // read-only indexing behaves as expected
  int const operator[] (std::string string) {
    if (indexes.count(string))
      return indexes[string];
    else
      return -1;
  };

  std::string operator[] (int index) {
    if (index < (int) strings.size() && index >= 0)
      return strings[index];
    else
      return 0;
  };

  bool has (std::string &string) {
    return indexes.count(string) > 0;
  };

  int add (std::string string) {
    if (indexes.count(string))
      return indexes[string];

    strings.push_back(string);
    indexes.insert(std::pair < std::string, int > (string, count));
    return count++;
  };
};


#endif /* HASHED_STRING_VECTOR_HPP */

