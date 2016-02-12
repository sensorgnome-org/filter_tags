#ifndef KNOWN_TAG_HPP
#define KNOWN_TAG_HPP

#include "filter_tags_common.hpp"

#include <unordered_map>
#include <unordered_set>

struct Known_Tag {

  // represents a database of tags we have "registered" (i.e. estimated
  // gaps, burst interval, and frequency offset from raw receiver audio)
  // The motus version uses only Lotek ID (which is what is seen in the input)
  // and motusID.  The codeset is assumed constant for a run of filter_tags,
  // which is enforced by the harness code.

public:

  Lotek_Tag_ID		lid;  // lotek ID (e.g. 1..521 for the Lotek4 codeset)
                              // for matching against input
  Motus_Tag_ID          mid;  // motus ID of tag, used for output
  Nominal_Frequency_kHz freq; // nominal transmit frequency
  float			bi;   // burst interval, in seconds


private:
  static std::unordered_set < Motus_Tag_ID > all_motusIDs;       // full ID list

public:

  Known_Tag(){};

  Known_Tag(Lotek_Tag_ID lid, Motus_Tag_ID mid, Nominal_Frequency_kHz freq, float bi);
};

typedef std::unordered_set < Known_Tag * > Tag_Set; 

#endif // KNOWN_TAG_HPP
