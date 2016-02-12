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

typedef int Lotek_Tag_ID;
static const Lotek_Tag_ID BOGUS_LOTEK_TAG_ID = -1;

// a motus tag ID is an integer key into the motus tags database

typedef int Motus_Tag_ID;

// a set of Lotek tag IDs
typedef std::unordered_set < Lotek_Tag_ID > Lotek_ID_Set;

class Known_Tag;
typedef Known_Tag * Tag_ID;
static const Tag_ID BOGUS_TAG_ID = 0;

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
