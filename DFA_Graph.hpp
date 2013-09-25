#ifndef DFA_GRAPH_HPP
#define DFA_GRAPH_HPP

#include "filter_tags_common.hpp"

#include "DFA_Node.hpp"

#include <vector>

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

public:

  typedef std::map < Tag_ID_Set, DFA_Node * > Node_For_IDs;
  

private:
  // the max depth of this graph
  
  unsigned int max_depth;

  // the root node, where any DFA begins its life

  DFA_Node * root;

  // For each depth, a map from sets of tag IDs to DFA_Node
  // this is only used in constructing the tree, but we don't
  // bother deleting.

  std::vector < Node_For_IDs > node_for_set_at_depth;

public:

  DFA_Graph(unsigned int max_depth);

  void add_tag_to_root (Tag_ID &t); // add a tag to the root of this graph

  DFA_Node *get_root();

  // iterators for nodes at each depth

  Node_For_IDs :: const_iterator begin_nodes_at_depth (unsigned int depth);

  Node_For_IDs :: const_iterator end_nodes_at_depth (unsigned int depth);

  // grow the DFA_Graph from a node via an interval_map; edges are added
  // between the specified node and (possibly new nodes) at the specified
  // depth.  "Edges" are really (interval < Gap > , Node*), collected
  // in an interval_map.

  void grow(DFA_Node *p, interval_map < Gap, Tag_ID_Set > &s, unsigned int depth);

  std::vector < Node_For_IDs > & get_node_for_set_at_depth();
};

#endif // DFA_GRAPH_HPP
