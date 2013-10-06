#include "Tag_Database.hpp"
#include <sstream>

Tag_Database::Tag_Database(string filename) {

  ifstream inf(filename.c_str(), ifstream::in);
  char buf[MAX_LINE_SIZE + 1];

  inf.getline(buf, MAX_LINE_SIZE);
  if (string(buf) != "\"proj\",\"id\",\"tagFreq\",\"bi\"")
    throw std::runtime_error("Tag file header missing or incorrect\n");

  int num_lines = 1;
  while (! inf.eof()) {
    inf.getline(buf, MAX_LINE_SIZE);
    if (inf.gcount() == 0)
      break;
    char proj[MAX_LINE_SIZE+1];
    int id;
    float freq_MHz;
    float bi;

    int num_par = sscanf(buf, "\"%[^\"]\",%d,%f,%f", proj, &id, &freq_MHz, &bi);
    if (num_par < 4) {
      std::ostringstream msg;
      msg << "Tag database file incomplete or corrupt at line " << (num_lines+1) << ", with only " << num_par << " parameters parsed successfully.\n";
      throw std::runtime_error(msg.str());
    }
    ++ num_lines;
    Nominal_Frequency_kHz nom_freq = Freq_Setting::as_Nominal_Frequency_kHz(freq_MHz);

    if (nominal_freqs.count(nom_freq) == 0) {
      // we haven't seen this nominal frequency before
      // add it to the list and create a place to hold stuff
      nominal_freqs.insert(nom_freq);
      tags[nom_freq] = Tag_Set();
    }
    if (tags[nom_freq].count(id) == 0) {
      Known_Tag t(id, string(proj), nom_freq, bi);
      tags[nom_freq][id] = t;
    } else {
      std::cerr << "Ignoring duplicated data for tag ID == " << id << "\nAlready have tag " << id << " with proj='" << tags[nom_freq][id].proj << "' at freq=" << tags[nom_freq][id].freq << "MHz\n";
    } 
  };
  if (tags.size() == 0)
    throw std::runtime_error("No tags registered.");
};

Freq_Set & Tag_Database::get_nominal_freqs() {
  return nominal_freqs;
};

Tag_Set *
Tag_Database::get_tags_at_freq(Nominal_Frequency_kHz freq) {
  return & tags[freq];
};

Known_Tag *
Tag_Database::get_tag(Nominal_Frequency_kHz freq, Tag_ID id) {
  return & tags[freq][id];
};
  

