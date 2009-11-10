PROGRAM_SOURCES=$(wildcard *.cpp)
PROGRAMS=$(PROGRAM_SOURCES:.cpp=)

.PHONY : $(PROGRAMS)
$(PROGRAMS):
	@(cd ..; make -s programs/$@)
	@./$@
