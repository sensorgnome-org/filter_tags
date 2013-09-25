#ifndef ANTENNA_HPP
#define ANTENNA_HPP

#include "filter_tags_common.hpp"
#include <vector>
#include <unordered_map>

// maintain a bidirectional map between antenna labels and integer codes
class {

private:
  static int count;
  static std::vector < std::string > labels;
  static std::unordered_map < std::string, int > codes;

public:
  static int label_to_code(std::string label);
  static std::string * code_to_label(int code);
}


#endif /* ANTENNA_HPP */

