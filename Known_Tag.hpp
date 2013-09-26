#ifndef KNOWN_TAG_HPP
#define KNOWN_TAG_HPP

#include "filter_tags_common.hpp"

#include <unordered_map>

struct Known_Tag {

  // represents a database of tags we have "registered" (i.e. estimated
  // gaps, burst interval, and frequency offset from raw receiver audio)

public:

  Tag_ID		id;				// full tag ID (Lotek ID is id % 1000)
  std::string		proj;				// project name
  Nominal_Frequency_kHz freq;				// nominal transmit frequency
  float			bi;	                        // burst interval, in seconds

  Known_Tag(){};

  Known_Tag(Tag_ID id, std::string proj, Nominal_Frequency_kHz freq, float bi);

  Lotek_Tag_ID get_lotek_ID();
};

typedef std::unordered_map < Tag_ID, Known_Tag > Tag_Set; 

#endif // KNOWN_TAG_HPP
