#include "Hit.hpp"
#include "Run_Foray.hpp"

Hit::Hit(double ts, Lotek_Tag_ID lid, int ant_code, short sig, unsigned int dtaline, Frequency_MHz ant_freq) :
  ts(ts),
  lid(lid),
  ant_code(ant_code),
  sig(sig),
  dtaline(dtaline),
  ant_freq(ant_freq)
{ 
  static Seq_No seq_no = 0;

  this->seq_no = ++seq_no;
};

Hit Hit::make(double ts, Lotek_Tag_ID lid, int ant_code, short sig, unsigned int dtaline, Frequency_MHz ant_freq) {
  return Hit(ts, lid, ant_code, sig, dtaline, ant_freq);
};

void Hit::dump() {
  // 14 digits in timestamp output yields 0.1 ms precision
  std::cout << std::setprecision(14) << ts << std::setprecision(3) << ',' << lid << ',' << Run_Foray::ant_codes[ant_code] << sig << endl;
};
