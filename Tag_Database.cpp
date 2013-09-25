#include "Tag_Database.hpp"

Tag_Database::Tag_Database(string filename) :
  tags(),
  nominal_freqs()
{

  ifstream inf(filename.c_str(), ifstream::in);
  char buf[MAX_LINE_SIZE + 1];

  inf.getline(buf, MAX_LINE_SIZE);
  if (string(buf) != "\"proj\",\"id\",\"tagFreq\",\"fcdFreq\",\"g1\",\"g2\",\"g3\",\"bi\",\"dfreq\",\"g1.sd\",\"g2.sd\",\"g3.sd\",\"bi.sd\",\"dfreq.sd\",\"filename\"")
    throw std::runtime_error("Tag file header missing or incorrect\n");

  int num_lines = 1;
  while (! inf.eof()) {
    inf.getline(buf, MAX_LINE_SIZE);
    if (inf.gcount() == 0)
      break;
    char proj[MAX_LINE_SIZE+1], filename[MAX_LINE_SIZE];
    int id;
    float freq_MHz, fcd_freq, gaps[4], dfreq, g1sd, g2sd, g3sd, bisd, dfreqsd;
    int num_par = sscanf(buf, "\"%[^\"]\",%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,\"%[^\"]\"", proj, &id, &freq_MHz, &fcd_freq, &gaps[0], &gaps[1], &gaps[2], &gaps[3], &dfreq, &g1sd, &g2sd, &g3sd, &bisd, &dfreqsd, filename);
    if (num_par < 15) {
      std::cerr << "error at line " << num_lines << " with only " << num_par << " parameters parsed.\n";
      throw std::runtime_error("Tag file line incomplete or corrupt\n");
    }
    ++ num_lines;
    Nominal_Frequency_kHz nom_freq = Freq_Setting::as_Nominal_Frequency_kHz(freq_MHz);
    // convert gaps to seconds

    Tag_Group tg (nom_freq + (id % 1000) * 1000000);

    if (tags.count(tg)) {
      if (nominal_freqs.count(nom_freq) == 0) {
      // we haven't seen this nominal frequency before; add it to the list
        nominal_freqs.insert(nom_freq);
      }
      // this is a new (nominal freq, Lotek ID) pair; create a set for similar tags
      tags[tg] = Tag_Set();
    }
    Known_Tag t(id, string(proj), freq_MHz, gaps[3]);  // gaps[3] is the burst interval
    tags[tg][id] = t;
    tags_by_id[(Tag_ID) id] = tags[tg][id]; // take address of member in container, not temporary local "t"
  };
  if (tags.size() == 0)
    throw std::runtime_error("No tags registered.");
};

Freq_Set & Tag_Database::get_nominal_freqs() {
  return nominal_freqs;
};

Tag_Set *
Tag_Database::get_tags_at_freq_with_Lotek_ID(Nominal_Frequency_kHz freq, Lotek_Tag_ID lid) {
  return & tags[Tag_Group(freq + 1000000 * lid)];
};


Known_Tag &
Tag_Database::get_tag_with_id(Tag_ID tid) {
  return tags_by_id[tid];
};
