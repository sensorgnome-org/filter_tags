#ifndef DFA_NODE_HPP
#define DFA_NODE_HPP

#include "filter_tags_common.hpp"

#include <boost/icl/interval_map.hpp>
using namespace boost::icl;

class DFA_Node {

  friend class DFA_Graph;

private:
  typedef interval_map < Gap, DFA_Node * > Edges;
  typedef Edges :: iterator Edge_iterator;
  typedef Edges :: const_iterator Const_Edge_iterator;

  // fundamental structure

  unsigned int	depth;		// depth of this node in the tree
  Tag_ID_Set	ids;		// ids of tags in this node; these would all correspond to
                                // tags with the same lotek ID, but possibly different burst intervals
  Edges		edges;		// interval map from gap to other nodes
  Gap	        max_age;	// maximum time a DFA can remain in
                                // this state.  If a DFA in this state
                                // goes longer than this without
                                // adding a burst, then its run is terminated and it is destroyed.

public:  
  DFA_Node(unsigned int depth);

  DFA_Node(unsigned int depth, const Tag_ID_Set &ids);

  DFA_Node * next (Gap bi);

  bool is_unique();

  void set_max_age();

  Gap get_max_age();

  Tag_ID get_id();

  void dump(ostream & os, string indent = "", string indent_change = "   ");
    
};

#endif // DFA_NODE_HPP

