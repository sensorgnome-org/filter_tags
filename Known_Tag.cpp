#include "Known_Tag.hpp"

#include <sstream>
#include <math.h>

Known_Tag::Known_Tag(Lotek_Tag_ID lid, string proj, Nominal_Frequency_kHz freq, float bi) :
  lid(lid),
  proj(proj),
  freq(freq),
  bi(bi)
{
  // generate a full ID string  Proj#Lid@NOMFREQ:BI
  std::ostringstream fid;
  fid << proj << '#' << std::setprecision(4) << lid << '@' << std::setprecision(6) << freq / 1000.0 << ':' << round(10 * bi) / 10;
  fullID = fid.str();
  if (all_fullIDs.count(fullID)) {
    std::cerr << "Warning - two very similar tags in project " << proj << ":\nLotek ID: " << lid << "; frequency: " << freq << " MHz; burst interval: " << round(10 * bi) / 10 << " sec\nAppending '!' to fullID of second one.\n";
    while (all_fullIDs.count(fullID)) {
      fullID += '!';
    };
  }
  all_fullIDs.insert(fullID);
};

std::unordered_set < std::string > 
Known_Tag::all_fullIDs;       // will be populated as tags database is built
