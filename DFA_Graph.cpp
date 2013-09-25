#include "DFA_Graph.hpp"

DFA_Graph::DFA_Graph(unsigned int max_depth) :
  max_depth(max_depth),
  root(new DFA_Node(0)),

  // NB: preallocate the vector of maps so that iterators to particular
  // maps are not invalidated by reallocation of the vector as it grows.

  node_for_set_at_depth(max_depth + 1) 
{};

void DFA_Graph::add_tag_to_root (Tag_ID & id) {
  // add a tag to the root node of a DFA
  // Root is the only node at depth 0

  // sanity check: all tags at the root node should have the
  // same Lotek tag ID (but vary in actual tag ID)
  // node_for_set_at_depth[0][s] = root;
  root->ids.insert(id);
};

DFA_Node *DFA_Graph::get_root() {
  return root;
};

// iterators for nodes at each depth

DFA_Graph::Node_For_IDs :: const_iterator DFA_Graph::begin_nodes_at_depth (unsigned int depth) {
  return node_for_set_at_depth[depth].begin();
};

DFA_Graph::Node_For_IDs :: const_iterator DFA_Graph::end_nodes_at_depth (unsigned int depth) {
  return node_for_set_at_depth[depth].end();
};

// grow the DFA_Graph from a node via an interval_map; edges are added
// between the specified node and (possibly new nodes) at the specified
// depth.  "Edges" are really (interval < Gap > , Node*), collected
// in an interval_map.

void DFA_Graph::grow(DFA_Node *p, interval_map < Gap, Tag_ID_Set > &s, unsigned int depth) {

  if (! root)
    root = new DFA_Node(0);

  if (!p)
    p = root;

  if (depth >= node_for_set_at_depth.size())
    node_for_set_at_depth.push_back(Node_For_IDs());

  Node_For_IDs &node_for_set = node_for_set_at_depth[depth];

  // for each unique Tag_ID_Set in the interval_map,
  // create a new node and link to it via the edges interval_map
    
  for (interval_map < Gap, Tag_ID_Set > :: const_iterator it = s.begin(); it != s.end(); ++it) {
    if (! node_for_set.count(it->second))
      node_for_set[it->second] = new DFA_Node(p->depth + 1, it->second);
    p->edges.set(make_pair(it->first, node_for_set[it->second]));
  }

  // ensure the node's max age is correct, given we may have added
  // new edges

  p->set_max_age();
};

std::vector < DFA_Graph:: Node_For_IDs > & DFA_Graph::get_node_for_set_at_depth() {
  return node_for_set_at_depth;
};
