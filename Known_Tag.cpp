#include "Known_Tag.hpp"

Known_Tag::Known_Tag(Tag_ID id, string proj, Nominal_Frequency_kHz freq, float bi) :
  id(id),
  proj(proj),
  freq(freq),
  bi(bi)
{
};

Lotek_Tag_ID
Known_Tag::get_lotek_ID() {
  return id % 1000;
};

