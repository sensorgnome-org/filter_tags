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
    nominal_freqs.insert(nom_freq);
    tags[id] = Known_Tag(id, string(proj), nom_freq, gaps[3]); // gaps[3] is the burst interval
  };
  if (tags.size() == 0)
    throw std::runtime_error("No tags registered.");
};

Freq_Set & Tag_Database::get_nominal_freqs() {
  return nominal_freqs;
};

Tag_Set::iterator
Tag_Database::begin() {
  return tags.begin();
};

Tag_Set::iterator
Tag_Database::end() {
  return tags.end();
};


Known_Tag *
Tag_Database::get_tag(Tag_ID tid) {
  return & tags[tid];
};
