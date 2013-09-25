#ifndef RUN_CANDIDATE_HPP
#define RUN_CANDIDATE_HPP

#include <map>
#include <list>

#include "filter_tags_common.hpp"

#include "DFA_Node.hpp"
#include "Hit.hpp"
#include "Freq_Setting.hpp"

class Run_Finder;

class Run_Candidate {

  /* an automaton walking the DFA graph to find valid runs of detections from a single physical tag */

private: 
  // fundamental structure

  Run_Finder         *owner;          // which Run_Finder owns me?
  DFA_Node           *state;          // where in the appropriate DFA I am
  Hit_Buffer          hits;           // hits in the path so far
  Timestamp           last_ts;        // timestamp of last burst
  Timestamp           last_dumped_ts; // timestamp of last dumped burst (used to calculate burst slop when dumping)
  Tag_ID              tag_id;         // tag ID of this run
  bool                confirmed;      // is the current tag ID confirmed?
  unsigned long long  run_id;         // unique ID for this run
  unsigned int        in_a_row;       // counter of bursts in this run
  Gap                 bi;             // the burst interval, in seconds, for this tag

  static const float BOGUS_BURST_SLOP; // burst slop reported for first burst of run (where we don't have a previous burst)  Doesn't really matter, since we can distinguish this situation in the data by "pos.in.run==1"

public:

  static unsigned int hits_to_confirm_id; // how many hits must be seen before an ID level moves to confirmed?  
 
  Run_Candidate(Run_Finder *owner, DFA_Node *state, const Hit &h);

  bool has_same_id_as(Run_Candidate &tf);

  bool shares_any_hits(Run_Candidate &tf);

  bool is_too_old_given_hit_time(const Hit &h);

  DFA_Node * advance_by_hit(const Hit &h);

  bool add_hit(const Hit &h, DFA_Node *new_state);

  Tag_ID get_tag_id();

  bool is_confirmed();

  void clear_hits();

  void set_bi(float bi);
 
  void dump_hits(ostream *os, string prefix="");

  static void set_hits_to_confirm_id(unsigned int n);
};

#endif // RUN_CANDIDATE_HPP
