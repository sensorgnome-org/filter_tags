## Makefile for linux

## PROFILING FLAGS (uncomment to enable profiling)
## PROFILING=-g -pg

## DEBUG FLAGS:
##CPPFLAGS=-Wall -g3 -std=c++0x $(PROFILING)

## PRODUCTION FLAGS:
CPPFLAGS=-Wall -O3 -std=c++0x $(PROFILING)


all: filter_tags

clean:
	rm -f *.o filter_tags

Freq_Setting.o: Freq_Setting.cpp Freq_Setting.hpp filter_tags_common.hpp

Freq_History.o: Freq_History.cpp Freq_History.hpp filter_tags_common.hpp

DFA_Node.o: DFA_Node.cpp DFA_Node.hpp filter_tags_common.hpp

DFA_Graph.o: DFA_Graph.cpp DFA_Graph.hpp filter_tags_common.hpp

Known_Tag.o: Known_Tag.cpp Known_Tag.hpp filter_tags_common.hpp

Tag_Database.o: Tag_Database.cpp Tag_Database.hpp filter_tags_common.hpp

Hit.o: Hit.cpp Hit.hpp filter_tags_common.hpp

Run_Candidate.o: Run_Candidate.hpp Run_Candidate.cpp Run_Finder.hpp filter_tags_common.hpp

Run_Finder.o: Run_Finder.hpp Run_Finder.cpp Run_Candidate.hpp filter_tags_common.hpp

Run_Foray.o: Run_Foray.hpp Run_Foray.cpp filter_tags_common.hpp

filter_tags.o: filter_tags.cpp filter_tags_common.hpp Freq_History.hpp Freq_Setting.hpp DFA_Node.hpp DFA_Graph.hpp Known_Tag.hpp Tag_Database.hpp Hit.hpp Run_Candidate.hpp Run_Finder.hpp Run_Foray.hpp Hashed_String_Vector.hpp

filter_tags: Freq_Setting.o Freq_History.o DFA_Node.o DFA_Graph.o Known_Tag.o Tag_Database.o Hit.o Run_Candidate.o Run_Finder.o filter_tags.o Run_Foray.o
	g++ $(PROFILING) -o filter_tags_unifile $^
