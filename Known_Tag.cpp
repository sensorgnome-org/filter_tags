#include "Known_Tag.hpp"

#include <sstream>
#include <math.h>

Known_Tag::Known_Tag(Lotek_Tag_ID lid, Motus_Tag_ID mid, Nominal_Frequency_kHz freq, float bi) :
  lid(lid),
  mid(mid),
  freq(freq),
  bi(bi)
{
  if (all_motusIDs.count(mid)) {
    std::cerr << "Warning - duplicate motus ID.\n");
  all_motusIDs.insert(mid);
};

std::unordered_set < Motus_Tag_ID > 
Known_Tag::all_motusIDs;       // will be populated as tags database is built
