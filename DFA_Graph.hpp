#ifndef DFA_GRAPH_HPP
#define DFA_GRAPH_HPP

#include "filter_tags_common.hpp"

#include "DFA_Node.hpp"
#include "Known_Tag.hpp"

#include <vector>
#include <unordered_set>
#include <unordered_map>

// a type to map sets of tag Ids to DFA nodes
typedef std::map < Tag_ID_Set, DFA_Node * > Node_Map;

class DFA_Graph {
  // the graph representing a DFA for recognizing sequences of
  // runs from a tag with the same ID and burst interval

  // The root node is where all runs start; if a DFA is at root,
  // that means a single tag hit with the given ID and antenna
  // frequency has been seen.  We traverse from one node to another
  // if a burst of the correct ID on the right frequency has been
  // seen, and if the burst interval is compatible with the
  // range of burst intervals labelling the edge.

  // So there is one DFA_Graph per (Nominal Frequency, Lotek tag ID) pair.

  // The Run_Finder class gets access to the root and sets of nodes at each depth.

  friend class Run_Finder;

public:


protected:
  // the max depth of this graph

  unsigned int max_depth;

  // the root node, where any DFA begins its life

  DFA_Node * root;

  // at each depth, there is a map from sets of tag IDs
  // to nodes


  // For each depth, the nodes are in a Node_Map, indexed
  // by Tag_ID_Set
  // These are collected into a vector, indexed by depth.
  std::vector < Node_Map > N;

  // The set of tags for this graph

  Tag_ID_Set tags;

public:

  DFA_Graph(unsigned int max_depth=0);

  void add_tag (Known_Tag *t); // add a tag to this graph's tag set and root node

  void setup_root();

  DFA_Node *get_root();

  // grow the DFA_Graph from a node via an interval_map; edges are added
  // between the specified node and (possibly new nodes) at the specified
  // depth.  "Edges" are really (interval < Gap > , Node*), collected
  // in an interval_map.

  void grow(DFA_Node *p, interval_map < Gap, Tag_ID_Set > &s, unsigned int depth);
};

#endif // DFA_GRAPH_HPP
