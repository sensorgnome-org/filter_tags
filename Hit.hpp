#ifndef HIT_HPP
#define HIT_HPP

#include "filter_tags_common.hpp"

#include <map>

struct Hit {

  // a tag detection from the lotek receiver, as reported by the R function readDTA()

  typedef long long Seq_No;

public:

  // data from the hit detector

  double		ts;		// timestamp, in seconds past the Origin (to a precision of 0.0001 seconds)
  Lotek_Tag_ID          lid;            // Lotek tag ID
  int                   ant_code;       // antenna code 
  float		        sig;		// estimate of hit strength, in dB max
  float                 lat;            // latitude, if available
  float                 lon;            // longitude, if available
  Frequency_MHz         ant_freq;	// frequency to which receiving antenna was tuned
  int                   codeset_id;     // code set ID

  // additional parameters for algorithmic use

  Seq_No	seq_no;     

private:
  Hit(double ts, Lotek_Tag_ID lid, int ant_code, float sig, float lat, float lon, Frequency_MHz ant_freq, int codeset_id);

public:
  Hit(){};

  static Hit make(double ts, Lotek_Tag_ID lid, int ant_code, float sig, float lat, float lon, Frequency_MHz ant_freq, int codeset_id);

  void dump();
};

typedef std::map < Hit::Seq_No, Hit > Hit_Buffer;
typedef Hit_Buffer :: iterator Hits_Iter;

#endif // HIT_HPP
