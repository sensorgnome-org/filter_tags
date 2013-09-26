#ifndef RUN_FORAY_HPP
#define RUN_FORAY_HPP

#include "filter_tags_common.hpp"

#include "Tag_Database.hpp"
#include "Run_Finder.hpp"
#include "Hashed_String_Vector.hpp"

/*
  Run_Foray - manager a collection of run finders searching the same data stream.
  Each run finder operates on a single nominal frequency, but across antennas, 
  since the Lotek receiver switches between antennas (they are not listened
  to simultaneously as with the sensorgnome.)
*/

class Run_Foray {

public:
  
  Run_Foray (Tag_Database &tags, std::istream * data, std::ostream * out);

  //  ~Run_Foray ();

  void start();
  Tag_Database tags; // registered tags on all known nominal frequencies

protected:
  static const int MAX_ANT_CODE_SIZE = 5; // max size (chars) of a lotek antenna code
  static const int MAX_CODESET_SIZE = 32; // max size (chars) of a lotek codeset id
  // settings

  std::istream * data; // stream from which data records are read
  std::ostream * out;  // stream to which tag ID hits are output 

  // runtime storage

  // count lines of input seen
  unsigned long long line_no;
  
  // we need a Run_Finder for each combination of port and nominal frequency
  // we'll use a map

  typedef std::map < Nominal_Frequency_kHz, Run_Finder * > Run_Finder_Map;

  Run_Finder_Map run_finders;

public:

  static Hashed_String_Vector ant_codes;
  static Hashed_String_Vector codeset_ids;
};


#endif // RUN_FORAY
