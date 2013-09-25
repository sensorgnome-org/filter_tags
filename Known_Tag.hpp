#ifndef KNOWN_TAG_HPP
#define KNOWN_TAG_HPP

#include "filter_tags_common.hpp"

#include <map>

struct Known_Tag {

  // represents a database of tags we have "registered" (i.e. estimated
  // gaps, burst interval, and frequency offset from raw receiver audio)

public:

  Tag_ID		id;				// full tag ID (Lotek ID is id % 1000)
  std::string		proj;				// project name
  Frequency_MHz		freq;				// nominal transmit frequency (MHz)
  float			bi;	                        // burst interval, in seconds

  Known_Tag(){};

  Known_Tag(Tag_ID id, std::string proj, Frequency_MHz freq, float bi);

};

typedef std::map < Tag_ID, Known_Tag > Tag_Set; 

#endif // KNOWN_TAG_HPP
