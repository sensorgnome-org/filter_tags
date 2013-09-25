#include "DFA_Node.hpp"

DFA_Node::DFA_Node(unsigned int depth) :
  depth(depth),
  ids(),
  edges(),
  max_age(-1)
{};

DFA_Node::DFA_Node(unsigned int depth, const Tag_ID_Set &ids) :
  depth(depth),
  ids(ids),
  edges(),
  max_age(-1)
{};

DFA_Node * DFA_Node::next (Gap bi) {

  // return the DFA_Node obtained by following the edge labelled "bi",
  // or NULL if no such edge exists.  i.e. move to the state representing
  // those tag IDs which are compatible with the current set of tag IDs
  // and with the specified the specified gap to the next burst.

  Const_Edge_iterator it = edges.find(bi);
  if (it == edges.end())
    return 0;
  else
    return it->second;
};

bool DFA_Node::is_unique() {

  // does this DFA state represent a single Tag ID?

  return ids.size() == 1;
};

void DFA_Node::set_max_age() {

  // set the maximum time before death for a DFA in this state
  // This is the longest time we can wait for a pulse that will
  // still lead to a valid NDFA node.

  if (edges.size() > 0)
    max_age = upper(edges.rbegin()->first);
  else
    max_age = -1;
};

Gap DFA_Node::get_max_age() {
  return max_age;
};

Tag_ID DFA_Node::get_id() {
  // only valid when is_unique() is true
  return *ids.begin();
};

void DFA_Node::dump(ostream & os, string indent, string indent_change) {
    
  // output the tree rooted at this node, appropriately indented,
  // to a stream

  os << indent <<  "NODE @ depth " << depth << " ; max age: " << max_age << "\n"
     << indent << ids << "\n" << indent << "Edges:" << "\n";
  for (Edge_iterator it = edges.begin(); it != edges.end(); ++it) {
    os << indent << it->first << "->";
    if (it->second->depth > depth) {
      os << endl;
      it->second->dump(os, indent + indent_change);
    } else {
      os << " Back to NODE @ depth " << it->second->depth << ":" << it->second->ids << endl;
    }
  }
};
