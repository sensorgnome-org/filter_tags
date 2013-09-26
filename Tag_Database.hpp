#ifndef TAG_DATABASE_HPP
#define TAG_DATABASE_HPP

#include "filter_tags_common.hpp"

#include "Freq_Setting.hpp"
#include "Known_Tag.hpp"

#include <unordered_map>
#include <utility>

class Tag_Database {

 private:

  Tag_Set tags; // owns the Known_Tags objects within it

  Freq_Set nominal_freqs;

public:
  Tag_Database (string filename);

  Freq_Set & get_nominal_freqs();

  Known_Tag * get_tag(Tag_ID tid);

  Tag_Set::iterator begin();

  Tag_Set::iterator end();

};

#endif // TAG_DATABASE_HPP
