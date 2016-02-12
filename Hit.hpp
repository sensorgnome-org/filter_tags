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
  short		        sig;		// estimate of hit strength, in lotek units
  unsigned int          dtaline;        // line in original .DTA source file
  Frequency_MHz         ant_freq;	// frequency to which receiving antenna was tuned

  // additional parameters for algorithmic use

  Seq_No	seq_no;     

private:
  Hit(double ts, Lotek_Tag_ID lid, int ant_code, short sig, unsigned int dtaline, Frequency_MHz ant_freq);

public:
  Hit(){};

  static Hit make(double ts, Lotek_Tag_ID lid, int ant_code, short sig, unsigned int dtaline, Frequency_MHz ant_freq);

  void dump();
};

typedef std::map < Hit::Seq_No, Hit > Hit_Buffer;
typedef Hit_Buffer :: iterator Hits_Iter;

#endif // HIT_HPP
