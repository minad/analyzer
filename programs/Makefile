PROGRAM_SOURCES=$(wildcard *.cpp)
PROGRAMS=$(PROGRAM_SOURCES:.cpp=)

all:
	@(cd ..; make -s)
	
.PHONY : $(PROGRAMS)
$(PROGRAMS):
	@(cd ..; make -s programs/$@)
	@./$@
