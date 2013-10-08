#ifndef KNOWN_TAG_HPP
#define KNOWN_TAG_HPP

#include "filter_tags_common.hpp"

#include <unordered_map>
#include <unordered_set>

struct Known_Tag {

  // represents a database of tags we have "registered" (i.e. estimated
  // gaps, burst interval, and frequency offset from raw receiver audio)

public:

  Lotek_Tag_ID		lid;				// lotek ID
  std::string		proj;				// project name
  Nominal_Frequency_kHz freq;				// nominal transmit frequency
  float			bi;	                        // burst interval, in seconds

  std::string           fullID;                         // full ID, used when printing tags

private:
  static std::unordered_set < std::string > all_fullIDs;       // full ID list

public:

  Known_Tag(){};

  Known_Tag(Lotek_Tag_ID lid, std::string proj, Nominal_Frequency_kHz freq, float bi);
};

typedef std::unordered_set < Known_Tag * > Tag_Set; 

#endif // KNOWN_TAG_HPP
