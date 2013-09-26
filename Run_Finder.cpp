#include "Run_Finder.hpp"

Run_Finder::Run_Finder (Run_Foray *owner) :
  owner(owner) 
{
};

Run_Finder::Run_Finder (Run_Foray *owner, Nominal_Frequency_kHz nom_freq, string prefix) :
  owner(owner),
  nom_freq(nom_freq),
  G(),
  cands(),
  burst_slop(default_burst_slop),
  burst_slop_expansion(default_burst_slop_expansion),
  max_skipped_bursts(default_max_skipped_bursts),
  prefix(prefix)
{
};

void 
Run_Finder::add_tag(Known_Tag * t)
{
  if (t->freq != nom_freq) 
    throw std::runtime_error("Internal error: attempt to add tag to Run_Finder on different nominal frequency!\n");

  Lotek_Tag_ID lid = t->get_lotek_ID();
  if (G.count(lid) == 0)
    G.insert(std::pair < Lotek_Tag_ID, DFA_Graph > (lid, DFA_Graph(Run_Candidate::hits_to_confirm_id)));
  G[lid].add_tag(t);
  if (cands.count(lid) == 0)
    cands[lid] = Cand_Set();
}

void
Run_Finder::setup_graphs() {
  // Create the DFA graphs for the database of registered tags
  // There is one graph for each set of tags having the same Lotek ID
  // and on the same frequency

  // The graphs have already had their root nodes created by calling add_tag
  // for each tag on this Run_Finder's frequency.
  
  // For depth up to Run_Candidate::hits_to_confirm_id, add appropriate nodes to the graph.
  // This is done breadth-first, but non-recursively because the DFA_Graph
  // class keeps a vector of nodes at each phase.
  // We start by looking at all nodes in the previous phase.
  // Each one represents a set of Tag_IDs.  We build an interval_map from
  // pulse gaps for the current phase (including slop) to subsets of these
  // Tag_IDs.

  // loop over each graph (i.e. each lotek ID)
  for (auto ig = G.begin(); ig != G.end(); ++ig) {
    DFA_Graph &g = ig->second;
    g.setup_root();

    // loop over each depth (i.e. breadth-first)
    for (unsigned int depth = 0; depth < g.max_depth; ++depth) {
      
      Node_Map & nm = g.N[depth];

      // loop over each node at this depth
      for (auto in = nm.begin(); in != nm.end(); ++in) {

        // a map of gap sizes to compatible tag IDs
        interval_map < Gap, Tag_ID_Set > m;

        // for each tag in this node, add edges for fuzzified
        // multiples of its burst interval
	  
        for (auto i = in->first.begin(); i != in->first.end(); ++i) {
          Tag_ID_Set id;
          id.insert(*i);
          Gap bi = g.tags[*i]->bi;
          for (unsigned int k = 1; k <= max_skipped_bursts; ++k) {
            Gap slop =  burst_slop + burst_slop_expansion * (k - 1);
            m.add(make_pair(interval < Gap > :: closed(bi * k - slop, bi * k + slop), id));
          }
        }

        // grow the node by this interval_map; Pulses at phase 2 *
        // PULSES_PER_BURST-1 are linked back to pulses at phase
        // PULSES_PER_BURST-1, so that we can keep track of runs of
        // consecutive bursts from a tag
      
        g.grow(in->second, m, (depth < g.max_depth - 1) ? depth + 1 : depth);
      }
    }

    // sanity check: for each node at max depth, ensure there's only one tag ID left
    Node_Map & nm = g.N[g.max_depth - 1];

    for (auto in = nm.begin(); in != nm.end(); ++in) {
      if (in->first.size() > 1) {
        std::ostringstream ids;
        ids << "Error: after " << g.max_depth << " bursts,\nthese tag IDs are not distinguishable with current parameters:\n";
        for (auto i = in->first.begin(); i != in->first.end(); ++i) {
          ids << "  " << (*i) << "\n";
        }
        throw std::runtime_error(ids.str());
      }
    }
  }
};


void
Run_Finder::set_default_burst_slop_ms(float burst_slop_ms) {
  default_burst_slop = burst_slop_ms / 1000.0;	// stored as seconds
};

void
Run_Finder::set_default_burst_slop_expansion_ms(float burst_slop_expansion_ms) {
  default_burst_slop_expansion = burst_slop_expansion_ms / 1000.0;   // stored as seconds
};

void
Run_Finder::set_default_max_skipped_bursts(unsigned int skip) {
  default_max_skipped_bursts = skip;
};

void
Run_Finder::set_out_stream(ostream * os) {
  out_stream = os;
};

void
Run_Finder::init() {
  setup_graphs();
  for (Graph_Map::iterator ig = G.begin(); ig != G.end(); ++ig) {
    std::cerr << ig->first << std::endl;
    ig->second.get_root()->dump(std::cerr);
  };

};

void
Run_Finder::output_header(ostream * out) {
  (*out) << "\"ant\",\"ts\",\"id\",\"freq\",\"freq.sd\",\"sig\",\"sig.sd\",\"noise\",\"run.id\",\"pos.in.run\",\"slop\",\"burst.slop\",\"hit.rate\",\"ant.freq\"" 
      
#ifdef FIND_TAGS_DEBUG
         << " ,\"p1\",\"p2\",\"p3\",\"p4\",\"ptr\""
#endif
      
         << std::endl;
};
    
void
Run_Finder::process(Hit &h) {
  /* 
     Process one tag hit from the input stream.  

     - destroy existing Run_Candidates which are too old, given the 
     "now" represented by this pulse's timestamp
    
     - for each remaining Run_Candidate, see whether hit pulse can be added
     (i.e. is compatible with an edge out of the DFA's current state,	and
     has sufficiently similar frequency offset)

     - if so, check whether the pulse confirms the Run_Candidate:

     - if so, kill any other Run_Candidates with the same ID

     - otherwise, if the hit was added to the candidate, but the candidate
     was unconfirmed before the addition, clone the original
     Run_Candidate (i.e. the one without the added hit)

     - if this run candidate is confirmed, and a hit was added,
     dump any hits so far

     - if this hit didn't confirm any run candidate, then start a new
     Run_Candidate with this hit
  */

  Cand_Set cloned_candidates;

  // unless this hit is used to confirm a run candidate (in which case it's
  // very likely to be part of that tag) we will start a new run candidate with it

  bool start_new_candidate_with_hit = true;

  Cand_Set & cs = cands[h.lid];

  for (Cand_Set::iterator ci = cs.begin(); ci != cs.end(); /**/ ) {

    if (ci->is_too_old_given_hit_time(h)) {

      if (ci->is_confirmed()) {
        ci->dump_hits(out_stream, prefix);
      }

      Cand_Set::iterator di = ci;
      ++ci;
      cs.erase(di);
      continue;
    }

    // see whether this Tag Candidate can accept this hit
    DFA_Node * next_state = ci->advance_by_hit(h);

    if (!next_state) {
      ++ci;
      continue;
    }

    // We will be adding the hit to this run candidate. 
    // If it is unconfirmed, clone it first.

    if (! ci->is_confirmed()) {
      // clone the candidate, without the added hit
      cloned_candidates.push_back(*ci);
    }

    if (ci->add_hit(h, next_state)) {
      // this run candidate has just been confirmed.
      // See what candidates should be deleted because they
      // have the same ID or share any pulses.

      for (Cand_Set::iterator cci = cs.begin(); cci != cs.end(); /**/ ) {
	if (cci != ci 
	    && (cci->has_same_id_as(*ci)
		|| cci->shares_any_hits(*ci)))
	  {
	    Cand_Set::iterator di = cci;
	    ++cci;
	    cs.erase(di);
	  } else {
	  ++cci;
	};
      }
	
      // dump all hits from this confirmed run
      ci->dump_hits(out_stream, prefix);
	
      // don't start a new candidate with this pulse
      start_new_candidate_with_hit = false;

      // don't try to add this pulse to other candidates
      break;
    }
    ++ci;
  } // continue trying letting other Run_Candidates try this pulse

    // add any cloned candidates to the end of the list
  cs.splice(cs.end(), cloned_candidates);

  // maybe start a new Run_Candidate with this pulse 
  if (start_new_candidate_with_hit) {
    cs.push_back(Run_Candidate(this, G[h.lid].get_root(), h));
  }
};

void
Run_Finder::end_processing() {
  // dump any confirmed candidates which have bursts

  for (Cand_Set_Map::iterator cm = cands.begin(); cm != cands.end(); ++cm) {

    Cand_Set &cs = cm->second;
    for (Cand_Set::iterator ci = cs.begin(); ci != cs.end(); ++ci ) {
      
      if (ci->is_confirmed()) {
        // we're about to dump bursts from a particular tag candidate
        // kill any others which have the same ID or share any pulses
	  
        for (Cand_Set::iterator cci = cs.begin(); cci != cs.end(); /**/ ) {
          if (cci != ci
              && (cci->has_same_id_as(*ci)
                  || cci->shares_any_hits(*ci)))
            { 
              Cand_Set::iterator di = cci;
              ++cci;
		
#ifdef FIND_TAGS_DEBUG
              std::cerr << "Killing id-matching too old candidate " << &(*di) << " with unique_id = " << di->unique_id << std::endl;
#endif
              cs.erase(di);
              continue;
            } else {
            ++cci;
          }
        }
        // dump the bursts
        ci->dump_hits(out_stream, prefix);
      }
    }
  }
};

Gap Run_Finder::default_burst_slop = 0.010; // 10 ms
Gap Run_Finder::default_burst_slop_expansion = 0.001; // 1ms = 1 part in 10000 for 10s BI
unsigned int Run_Finder::default_max_skipped_bursts = 60;

ostream * Run_Finder::out_stream = 0;
