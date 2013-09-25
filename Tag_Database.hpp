#ifndef TAG_DATABASE_HPP
#define TAG_DATABASE_HPP

#include "filter_tags_common.hpp"

#include "Freq_Setting.hpp"
#include "Known_Tag.hpp"

#include <unordered_map>
#include <utility>

class Tag_Database {

 private:
  // a tag group is a set of tags on the same nominal frequency and with the same Lotek ID
  // (but presumably distinguishable burst interval)
  // Tag_group is an unsigned int consisting of 1e6 * lotek_tag_id + nominalFreq_Khz
  // so XXXYYYZZZ where XXX is the Lotek tag ID, and YYY.ZZZ is the nominal frequency in MHz

  typedef unsigned int Tag_Group;
  typedef std::unordered_map < Tag_Group, Tag_Set > Tag_Set_Set;

  Tag_Set tags_by_id;
  Tag_Set_Set tags;

  Freq_Set nominal_freqs;

public:
  Tag_Database (string filename);

  Freq_Set & get_nominal_freqs();

  Tag_Set * get_tags_at_freq_with_Lotek_ID(Nominal_Frequency_kHz freq, Lotek_Tag_ID lid);

  Known_Tag & get_tag_with_id(Tag_ID tid);
};

#endif // TAG_DATABASE_HPP
