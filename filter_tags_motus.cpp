/*

  filter_tags_motus: filter sequences of tag hits from lotek receivers
  based on burst interval, and output results as detections of motusIDs

  Copyright 2013-2016 John Brzustowski

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <getopt.h>
#include <cmath>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <string.h>

#include "filter_tags_common.hpp"
#include "Freq_Setting.hpp"
//#include "Freq_History.hpp"
#include "Tag_Database.hpp"
#include "Hit.hpp"
#include "Run_Candidate.hpp"
#include "Run_Foray.hpp"

//#define FILTER_TAGS_DEBUG 

void 
usage() {
  puts (
	"Usage:\n"
	"    filter_tags_motus [OPTIONS] TAGDB.SQLITE OUTFILE.sqlite [INFILE.CSV]\n"
	"where:\n\n"

	"TAGDB.SQLITE is a file holding a table of registered tags\n"
	"    with (at least) these columns:\n"
        "        tagID, mfgID, tagFreq, period\n"
	"    with:\n"
	"        tagID: unique integer key for tag when registered with motus-wts.org\n"
	"        mfgID: Lotek codeset tag ID; integers as they appear in a Lotek .DTA file\n"
	"        tagFreq: (MHz) nominal tag frequency\n"
	"        period: (s) burst interval; time between first pulse of two consecutive bursts\n"

	"OUTFILE.sqlite is a database with various required tables; see the\n"
         "   source file filter_tags_motus.cpp for details\n\n"

	"INFILE.CSV is a file holding output from the readDTA() R function; stdin\n"
        " is used if not specified.  The format is:\n"
        "  [optional header line]\n"
        "  Data lines with the following comma-separated fields)\n"
        "     ts      - numeric GMT timestamp\n"
        "     id      - integer, no 999s\n"
        "     ant     - factor - Lotek antenna name - this field is treated as a string\n"
        "     sig     - signal strength, Lotek units\n"
        "     antfreq - antenna listening frequency, in MHz\n"
        
	"    If unspecified, tag hits are read from stdin\n\n"

	"and OPTIONS can be any of:\n\n"

	"-b, --burst-slop=BSLOP\n"
	"    how much to allow time between consecutive bursts\n"
	"    to differ from measured tag values, in millseconds.\n"
	"    default: 20 ms\n\n"

	"-B, --burst-slop-expansion=BSLOPEXP\n"
	"    how much to increase burst slop for each missed burst\n"
	"    in milliseconds; meant to allow for clock drift\n"
	"    default: 1 ms / burst\n\n"

	"-c, --hits-to-confirm=CONFIRM\n"
	"    how many hits must be detected before a run is confirmed.\n"
	"    default: 2\n\n"

        "-h  --help\n"
        "    print this help message\n\n"

	"-S, --max-skipped-bursts=SKIPS\n"
	"    maximum number of consecutive bursts that can be missing (skipped)\n"
	"    without terminating a run.  When using the pulses_to_confirm criterion\n"
	"    that number of pulses must occur with no gaps larger than SKIPS bursts\n"
	"    between them.\n"
	"    default: 60\n\n"

	);
}

int
main (int argc, char **argv) {
      enum {
	OPT_BURST_SLOP	         = 'b',
	OPT_BURST_SLOP_EXPANSION = 'B',
	OPT_HITS_TO_CONFIRM_ID      = 'c',
        COMMAND_HELP	         = 'h',
	OPT_MAX_SKIPPED_BURSTS   = 'S'
    };

    int option_index;
    static const char short_options[] = "b:B:c:hS:";
    static const struct option long_options[] = {
        {"burst-slop"		   , 1, 0, OPT_BURST_SLOP},
        {"burst-slop-expansion"    , 1, 0, OPT_BURST_SLOP_EXPANSION},
	{"hits-to-confirm"	   , 1, 0, OPT_HITS_TO_CONFIRM_ID},
        {"help"			   , 0, 0, COMMAND_HELP},
	{"max-skipped-bursts"      , 1, 0, OPT_MAX_SKIPPED_BURSTS},
        {0, 0, 0, 0}
    };

    int c;

    string tagdb_filename;
    string hits_filename = "";

    double burst_slop = 0.02;
    double burst_slop_expansion;
    int hits_to_confirm_id = 2;
    int max_skipped_bursts = 20;

    while ((c = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1) {
        switch (c) {
        case OPT_BURST_SLOP:
          burst_slop = atof(optarg);
	  break;
        case OPT_BURST_SLOP_EXPANSION:
          burst_slop_expansion = atof(optarg);
	  break;
	case OPT_HITS_TO_CONFIRM_ID:
          hits_to_confirm_id = atoi(optarg);
	  break;
        case COMMAND_HELP:
            usage();
            exit(0);
	case OPT_MAX_SKIPPED_BURSTS:
          max_skipped_bursts = atoi(optarg);
	  break;
        default:
            usage();
            exit(1);
        }
    }
    Run_Finder::set_default_burst_slop_ms(burst_slop);
    Run_Finder::set_default_burst_slop_expansion_ms(burst_slop_expansion);
    Run_Finder::set_default_max_skipped_bursts(max_skipped_bursts);
    Run_Candidate::set_hits_to_confirm_id(hits_to_confirm_id);

    if (optind == argc) {
      usage();
      exit(1);
    }

    tagdb_filename = string(argv[optind++]);

    if (optind == argc) {
      usage();
      exit(1);
    }

    string output_filename = string(argv[optind++]);

    std::istream * hits;
    if (optind < argc) {
      hits = new ifstream(argv[optind++]);
    } else {
      hits = & std::cin;
    }

    string program_name("find_tags_motus");
    string program_version(PROGRAM_VERSION); // defined in Makefile
    double program_build_ts = PROGRAM_BUILD_TS; // defined in Makefile

    try {
      // set options and parameters

      Tag_Database tag_db(tagdb_filename);

      DB_Filer dbf (output_filename, program_name, program_version, program_build_ts);
      dbf.add_param("burst_slop", burst_slop);
      dbf.add_param("burst_slop_expansion", burst_slop_expansion );
      dbf.add_param("hits_to_confirm_id", hits_to_confirm_id);
      dbf.add_param("max_skipped_bursts", max_skipped_bursts);

      Run_Candidate::set_filer(& dbf);

      // Freq_Setting needs to know the set of nominal frequencies
      Freq_Setting::set_nominal_freqs(tag_db.get_nominal_freqs());

      // open the input stream 

      Run_Foray foray(& tag_db, hits);

      foray.start();
    } catch (std::runtime_error e) {
      std::cerr << e.what();
      exit(1);
    }
}

