/*

  filtertags: filter sequences of tag hits from lotek receivers
  based on burst interval.

  Copyright 2013 John Brzustowski

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

//#define FIND_TAGS_DEBUG 

void 
usage() {
  puts (
	"Usage:\n"
	"    filter_tags [OPTIONS] TAGDB.CSV [TAGHITS.CSV]\n"
	"where:\n\n"

	"TAGDB.CSV is a file holding a table of registered tags\n"
	"    where each line is:\n"
        "        PROJ,ID,TAGFREQ,FCDFREQ,G1,G2,G3,BI,DFREQ,G1.SD,G2.SD,G3.SD,BI.SD,DFREQ.SD,FILENAME\n"
	"    with:\n"
	"        PROJ: quoted string identifying project these tags belong to\n"
	"        ID: integer giving manufacturer tag ID\n"
	"        TAGFREQ: (MHz) nominal tag frequency\n"
	"        FCDFREQ: (MHz) tuner frequency at which tag was registered\n"
	"        GN: (ms) time (gap) between pulses (N, N+1) in burst\n"
	"        BI: (s) time between consecutive bursts\n"
	"        DFREQ: (kHz) apparent offset of tag from tuner frequency\n"
	"        GN.SD: (ms) standard deviation of time between pulses (N, N+1)\n"
	"        BI.SD: (s) standard deviation of time between bursts\n"
	"        DFREQ.SD: (kHz) standard deviation of offset frequency\n"
	"        FILENAME: quoted string giving name of raw .WAV file (if any) recorded\n"
	"            at tag registration\n\n"

	"TAGHITS.CSV is a file holding output from the readDTA() R function which is in this format\n"
        "  [optional header line]\n"
        "  Data lines with the following comma-separated fields)\n"
        "     ts      - numeric GMT timestamp\n"
        "     id      - integer, no 999s\n"
        "     ant     - factor - Lotek antenna name - this field is treated as a string\n"
        "     sig     - signal strength, converted from Lotek units (0..255) to relative dB\n"
        "     lat     - if available, NA otherwise - this field is treated as a string\n"
        "     lon     - if available, NA otherwise - this field is treated as a string\n"
        "     antfreq - antenna listening frequency, in MHz\n"
        "     codeset - factor - Lotek codset name - this field is treated as a string\n\n"
        
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

	"-H, --header-only\n"
	"    output the header ONLY; does no processing.\n\n"

	"-n, --no-header\n"
	"    don't output the column names header; useful when output\n"
	"    is to be appended to an existing .CSV file.\n\n"

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
	OPT_HITS_TO_CONFIRM      = 'c',
        COMMAND_HELP	         = 'h',
	OPT_HEADER_ONLY	         = 'H',
	OPT_NO_HEADER	         = 'n',
	OPT_MAX_SKIPPED_BURSTS   = 'S'
    };

    int option_index;
    static const char short_options[] = "b:B:c:hHnS:";
    static const struct option long_options[] = {
        {"burst-slop"		   , 1, 0, OPT_BURST_SLOP},
        {"burst-slop-expansion"    , 1, 0, OPT_BURST_SLOP_EXPANSION},
	{"hits-to-confirm"	   , 1, 0, OPT_HITS_TO_CONFIRM},
        {"help"			   , 0, 0, COMMAND_HELP},
	{"header-only"		   , 0, 0, OPT_HEADER_ONLY},
	{"no-header"		   , 0, 0, OPT_NO_HEADER},
	{"max-skipped-bursts"      , 1, 0, OPT_MAX_SKIPPED_BURSTS},
        {0, 0, 0, 0}
    };

    int c;

    string tagdb_filename;
    string hits_filename = "";

    bool header_desired = true;

    while ((c = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1) {
        switch (c) {
        case OPT_BURST_SLOP:
	  Run_Finder::set_default_burst_slop_ms(atof(optarg));
	  break;
        case OPT_BURST_SLOP_EXPANSION:
	  Run_Finder::set_default_burst_slop_expansion_ms(atof(optarg));
	  break;
	case OPT_HITS_TO_CONFIRM:
	  Run_Candidate::set_hits_to_confirm_id(atoi(optarg));
	  break;
        case COMMAND_HELP:
            usage();
            exit(0);
	case OPT_HEADER_ONLY:
	  Run_Finder::output_header(&std::cout);
	  exit(0);
	case OPT_NO_HEADER:
	  header_desired = false;
	  break;
	case OPT_MAX_SKIPPED_BURSTS:
	  Run_Finder::set_default_max_skipped_bursts(atoi(optarg));
	  break;
        default:
            usage();
            exit(1);
        }
    }


    if (optind == argc) {
      usage();
      exit(1);
    }

    tagdb_filename = string(argv[optind++]);
    if (optind < argc) {
      hits_filename = string(argv[optind++]);
    }

    // set options and parameters

    Tag_Database tag_db (tagdb_filename);

    // Freq_Setting needs to know the set of nominal frequencies
    Freq_Setting::set_nominal_freqs(tag_db.get_nominal_freqs());

    // open the input stream 

    std::istream * hits;
    if (hits_filename.length() > 0) {
      hits = new ifstream(hits_filename.c_str());
    } else {
      hits = & std::cin;
    }

    if (hits->fail())
      throw std::runtime_error(string("Couldn't open input file ") + hits_filename);

    if (header_desired)
      Run_Finder::output_header(&std::cout);

    Run_Foray foray(tag_db, hits, & std::cout);

    foray.start();
}

