#include "DFA_Graph.hpp"

DFA_Graph::DFA_Graph(unsigned int max_depth) :
  max_depth(max_depth),

  // NB: preallocate the vector of node sets so that iterators to particular
  // sets are not invalidated by reallocation of the vector as it grows.
  
  root(0),
  N(max_depth),
  tags()
{
};

void 
DFA_Graph::add_tag (Known_Tag *t) {
  // add the tag to the root node, and add the known tag
  // to the list of all tags.

  // sanity check: we require all tags have the same Lotek ID
  if (tags.size() > 0 && (*tags.begin())->lid != t->lid)
    throw std::runtime_error("Internal error: attempt to add tags with different Lotek IDs to the same DFA_Graph!\n");

  tags.insert(t);
};

void 
DFA_Graph::setup_root() {
  if (! root) {
    // create the root with all known tag IDs in its set
    root = new DFA_Node(0);
    for (auto it = tags.begin(); it != tags.end(); ++it)
      root->ids.insert(*it);
    N[0][root->ids] = root;
  };
};
  
DFA_Node *
DFA_Graph::get_root() {
  return root;
};

// grow the DFA_Graph from a node via an interval_map; edges are added
// between the specified node and (possibly new nodes) at the specified
// depth.  "Edges" are really (interval < Gap > , Node*), collected
// in an interval_map.

void 
DFA_Graph::grow(DFA_Node *p, interval_map < Gap, Tag_ID_Set > &s, unsigned int depth) {

  if (!p)
    p = root;

  if (depth > N.size())
    throw std::runtime_error("Internal error: attempt to increase depth by more than 1.\n");

  if (depth == N.size())
    N[N.size()] = Node_Map();

  Node_Map & Nd = N[depth];

  // For each Tag_ID_Set in the interval_map, create a new node and
  // link to it via the edges interval_map.  This might lead to many
  // nodes with the same tag set, but it's not clear this is a
  // problem.
    
  for (auto it = s.begin(); it != s.end(); ++it) {
    DFA_Node *n;
    if (Nd.count(it->second) == 0) {
      Nd[it->second] = n = new DFA_Node(p->depth + 1, it->second);
    } else {
      n = Nd[it->second];
    }
    p->edges.set(make_pair(it->first, n));
  }

  // ensure the node's max age is correct, given we may have added
  // edges with larger gap sizes

  p->set_max_age();
};
