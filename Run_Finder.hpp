#ifndef RUN_FINDER_HPP
#define RUN_FINDER_HPP

#include "filter_tags_common.hpp"

#include "Known_Tag.hpp"
#include "Freq_Setting.hpp"
#include "DFA_Graph.hpp"
#include <unordered_map>
#include <list>

typedef std::unordered_map < Lotek_Tag_ID, DFA_Graph > Graph_Map;

// forward declaration for inclusion of Tag_Filter

class Run_Finder;
class Run_Candidate;
#include "Run_Candidate.hpp"

class Run_Finder {

  /*
    Find runs of tag detections from all antennas on a single nominal
    frequency.  Each run contains only bursts from a single physical
    tag, but there can be simultaneous runs of different tags.
  */

public:
  // - internal representation of tag database
  // the set of tags at a single nominal frequency and with the same Lotek ID is a "Tag_Set"

  Nominal_Frequency_kHz nom_freq;

  Tag_Set *tags;

  Graph_Map graph_by_lotek_id;  // a DFA graph for each lotek tag ID at this frequency

  // Set of running DFAs representing possible tags burst sequences

  typedef std::list < Run_Candidate > Candidate_Set;

  Candidate_Set cands;

  typedef std::list < Timestamp > Tag_Time_Buffer; // timestamps of recently seen hits on a tag

  typedef std::map < Tag_ID, Tag_Time_Buffer >  All_Tag_Time_Buffer; // timestamps of recent tag hit times by tag ID 

  All_Tag_Time_Buffer tag_times; // retain recent hit times for all tags

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

  ostream * out_stream;

  string prefix;   // prefix before each tag record (e.g. port number then comma)

  Run_Finder(){};

  Run_Finder (Nominal_Frequency_kHz nom_freq, Tag_Set * tags, string prefix="");

  void setup_graphs();

  static void set_default_burst_slop_ms(float burst_slop_ms);

  static void set_default_burst_slop_expansion_ms(float burst_slop_expansion_ms);

  static void set_default_max_skipped_bursts(unsigned int skip);

  void set_out_stream(ostream *os);

  void init();

  static void output_header(ostream *out);
    
  virtual void process (Hit &h);

  virtual void end_processing();

  void initialize_tag_buffers();

  void add_tag_hit_timestamp(Tag_ID id, Timestamp ts);

  float get_bi(Tag_ID tid);

};


#endif // RUN_FINDER_HPP
