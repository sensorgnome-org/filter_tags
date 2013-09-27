#ifndef FILTER_TAGS_COMMON_HPP
#define FILTER_TAGS_COMMON_HPP

#include <set>
#include <unordered_set>

const static unsigned int MAX_LINE_SIZE = 512;	// characters in a .CSV file line

// type representing a timestamp, in seconds since the Epoch (1 Jan 1970 GMT)

typedef double Timestamp;
const static Timestamp BOGUS_TIMESTAMP = -1; // timestamp representing not-a-timestamp

// type representing a VHF frequency, in MHz

typedef double Frequency_MHz;

// type representing a nominal frequency

typedef int Nominal_Frequency_kHz;

// Tag IDs: so far, Lotek tags have 3 digit IDs.  But we want to be
// able to distinguish among tags with the same Lotek ID but
// (sufficiently) different burst intervals.  So the tag database
// allows IDs greater than 999.  The last three digits of a tag ID
// correspond to the Lotek tag ID, and the thousands digits correspond
// to different physical tags with the same Lotek ID but
// distinguishable burst intervals.

typedef int Lotek_Tag_ID;  // only 3 digits
static const Lotek_Tag_ID BOGUS_LOTEK_TAG_ID = -1;

// a set of Lotek tag IDs
typedef std::unordered_set < Lotek_Tag_ID > Lotek_ID_Set;

typedef int Tag_ID;
static const Tag_ID BOGUS_TAG_ID = -1;

// a set of tag IDs;
typedef std::set< Tag_ID > Tag_ID_Set;
// an iterator for the set
typedef Tag_ID_Set::iterator Tag_ID_Iter;

// The type for interpulse gaps this should be able to represent a
// difference between two nearby Timestamp values We use float, which
// has sufficient precision here.

typedef float Gap;

// common standard stuff

#include <string>
using std::string;

#include <iostream>
using std::endl;
using std::ostream;

#include <iomanip>

#include <fstream>
using std::ifstream;

#include <stdexcept>

#endif // FILTER_TAGS_COMMON_HPP
