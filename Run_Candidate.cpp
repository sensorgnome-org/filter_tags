#include "Run_Candidate.hpp"

#include "Run_Foray.hpp"

Run_Candidate::Run_Candidate (Run_Finder *owner, DFA_Node *state, const Hit &h) :
  owner(owner),
  state(state),
  hits(),
  last_ts(h.ts),
  last_dumped_ts(BOGUS_TIMESTAMP),
  conf_tag(0),
  in_a_row(0),
  bi(0.0)
{
  static unsigned long long run_id_counter = 0;

  run_id = ++run_id_counter;
  hits[h.seq_no] = h;
};

bool Run_Candidate::has_same_id_as(Run_Candidate &tf) {
  Tag_ID id = get_tag_id();
  return id != BOGUS_TAG_ID && id == tf.get_tag_id();
};

bool Run_Candidate::shares_any_hits(Run_Candidate &tf) {
  // does this tag candidate use any of the hits
  // from a run accepted by another tag filter?

  Hit_Buffer::iterator ihit = tf.hits.begin();
  for (unsigned int i = 0; i < hits_to_confirm_id && ihit != tf.hits.end(); ++i, ++ihit)
    if (hits.count(ihit->first))
      return true;
  return false;
};

bool Run_Candidate::is_too_old_given_hit_time(const Hit &h) {
  return h.ts - last_ts > state->get_max_age();
};

DFA_Node * Run_Candidate::advance_by_hit(const Hit &h) {

  Gap gap = h.ts - last_ts;
	  
  // try walk the DFA with this gap
  return state->next(gap);
}

bool Run_Candidate::add_hit(const Hit &h, DFA_Node *new_state) {

  /*
    Add this hit to the run candidate, given the new state this
    will advance the DFA to.

    Return true if adding the hit confirms the tag ID.

  */

  hits[h.seq_no] = h;
  last_ts = h.ts;

  state = new_state;
      
  // does this new burst confirm the tagID ?

  if ((! conf_tag) && hits.size() >= hits_to_confirm_id) {
    conf_tag = owner->owner->tags.get_tag(state->get_ID());
    return true;
  }
  return false;
};

Tag_ID Run_Candidate::get_tag_id() {
  // get the ID of the tag associated with this candidate
  // if more than one tag is still compatible, this returns BOGUS_TAG_ID

  return conf_tag ? conf_tag->id : BOGUS_TAG_ID;
};

bool Run_Candidate::is_confirmed() {
  return conf_tag != 0;
};

void Run_Candidate::clear_hits() {
  // drop the most recent hit burst (presumably after
  // outputting it)

  hits.clear();
};

void Run_Candidate::set_bi(float bi) {
  this->bi = bi;
};

void Run_Candidate::dump_hits(ostream *os, string prefix) {
  // dump all hits in the run so far

  for (Hits_Iter ih = hits.begin(); ih != hits.end(); ++ih) {
    double bs;
    if (last_dumped_ts != BOGUS_TIMESTAMP) {
      double gap = ih->second.ts - last_dumped_ts;
      bs = fmodf(gap, bi) - bi;
    } else {
      bs = BOGUS_BURST_SLOP;
    }
    ++in_a_row;
    (*os) << prefix << std::setprecision(14) << ih->second.ts << std::setprecision(4)
          << ',' << Run_Foray::ant_codes[ih->second.ant_code]
	  << ',' << conf_tag->id
          << ",\"" << conf_tag->proj
	  << "\"," << ih->second.sig
	  << ',' << run_id
	  << ',' << in_a_row
	  << ',' << bs
	  << ',' << std::setprecision(6) << ih->second.ant_freq << std::setprecision(4) << std::endl;
    last_dumped_ts = ih->second.ts;
  }
  clear_hits();
};

void Run_Candidate::set_hits_to_confirm_id(unsigned int n) {
  hits_to_confirm_id = n;
};

unsigned int Run_Candidate::hits_to_confirm_id = 2; // default to at least 2 bursts required; choosing 1 would make filtering a NO-OP

const float Run_Candidate::BOGUS_BURST_SLOP = 0.0; // burst slop reported for first burst of ru
