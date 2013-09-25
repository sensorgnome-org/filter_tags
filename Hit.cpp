#include "Hit.hpp"
#include "Run_Foray.hpp"

Hit::Hit(double ts, Lotek_Tag_ID lid, int ant_code, float sig, float lat, float lon, Frequency_MHz ant_freq, int codeset_id) :
  ts(ts),
  lid(lid),
  ant_code(ant_code),
  sig(sig),
  lat(lat),
  lon(lon),
  ant_freq(ant_freq),
  codeset_id(codeset_id)
{ 
  static Seq_No seq_no = 0;

  this->seq_no = ++seq_no;
};

Hit Hit::make(double ts, Lotek_Tag_ID lid, int ant_code, float sig, float lat, float lon, Frequency_MHz ant_freq, int codeset_id) {
  return Hit(ts, lid, ant_code, sig, lat, lon, ant_freq, codeset_id);
};

void Hit::dump() {
  // 14 digits in timestamp output yields 0.1 ms precision
  std::cout << std::setprecision(14) << ts << std::setprecision(3) << ',' << lid << ',' << Run_Foray::ant_codes[ant_code] << sig << endl;
};
