#ifndef RUN_FINDER_HPP
#define RUN_FINDER_HPP

#include "filter_tags_common.hpp"

#include "Known_Tag.hpp"
#include "Freq_Setting.hpp"
#include "DFA_Graph.hpp"
#include "DFA_Node.hpp"
#include <unordered_map>
#include <list>

typedef std::unordered_map < Lotek_Tag_ID, DFA_Graph > Graph_Map;

// forward declaration for inclusion of Tag_Filter

class Run_Foray;
class Run_Finder;
class Run_Candidate;
#include "Run_Candidate.hpp"

// Set of running DFAs representing possible tags burst sequences
typedef std::list < Run_Candidate > Cand_Set;

// Map from Lotek ID to sets of Run_Candidates
typedef std::unordered_map < Lotek_Tag_ID, Cand_Set > Cand_Set_Map;

class Run_Finder {

  /*
    Find runs of tag detections from all antennas on a single nominal
    frequency.  Each run contains only bursts from a single physical
    tag, but there can be simultaneous runs of different tags.
  */

public:
  Run_Foray * owner;

  static std::unordered_set < Lotek_Tag_ID > tags_not_in_db;

  // - internal representation of tag database
  // the set of tags at a single nominal frequency and with the same Lotek ID is a "Tag_Set"

  Nominal_Frequency_kHz nom_freq;

  Graph_Map G;  // a DFA graph for each lotek tag ID at this frequency

  Cand_Set_Map cands; // for each Lotek ID, a set of run candidates

  // algorithmic parameters

  Gap burst_slop;	// (seconds) allowed slop in timing between
                        // consecutive tag bursts, in seconds this is
                        // meant to allow for measurement error at tag
                        // registration and detection times

  static Gap default_burst_slop;


  Gap burst_slop_expansion; // (seconds) how much slop in timing
			    // between tag bursts increases with each
  // skipped pulse; this is meant to allow for clock drift between
  // the tag and the receiver.
  static Gap default_burst_slop_expansion;

  // how many consecutive bursts can be missing without terminating a
  // run?

  unsigned int max_skipped_bursts;
  static unsigned int default_max_skipped_bursts;

  // output parameters

  static ostream * out_stream;

  string prefix;   // prefix before each tag record (e.g. port number then comma)

  Run_Finder(Run_Foray * owner);

  Run_Finder(Run_Foray * owner, Nominal_Frequency_kHz nom_freq, string prefix="");

  void add_tag(Known_Tag * t); // add a known tag to this run finder; it will be on the same
                               // frequency as the run finder

  void setup_graphs(); // after all known tags for this frequency have been added, this creates
  // the corresponding DFA graphs

  static void set_default_burst_slop_ms(float burst_slop_ms);

  static void set_default_burst_slop_expansion_ms(float burst_slop_expansion_ms);

  static void set_default_max_skipped_bursts(unsigned int skip);

  static void set_out_stream(ostream *os);

  void init();

  static void output_header(ostream *out);
    
  virtual void process (Hit &h);

  virtual void end_processing();

};


#endif // RUN_FINDER_HPP
