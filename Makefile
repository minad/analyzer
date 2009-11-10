BUILD_DIR=.build/
PROGRAMS_DIR=programs/
LIB_SOURCES=$(wildcard lib/*.cpp)
PROGRAM_SOURCES=$(wildcard $(PROGRAMS_DIR)*.cpp)
SOURCES=$(LIB_SOURCES) $(PROGRAM_SOURCES)
LIB_OBJECTS=$(addprefix $(BUILD_DIR),$(LIB_SOURCES:.cpp=.o))
CXXFLAGS=-Wall -g -Ilib
LDFLAGS=-Wall -lgsl -lboost_iostreams -lboost_regex
PROGRAMS=$(PROGRAM_SOURCES:.cpp=)
DEPEND_FILE=.make.depend

.PHONY : all depend clean mrproper

all: $(PROGRAMS)

depend:
	@echo "Find dependencies"
	@makedepend -p $(BUILD_DIR) -f - -- -MM $(CXXFLAGS) -- $(SOURCES) > $(DEPEND_FILE) 2>/dev/null

clean:
	@echo "Cleaning up"
	@rm -rf $(BUILD_DIR)

mrproper:
	@echo "Cleaning up everything"
	@rm -rf $(BUILD_DIR) $(PROGRAMS) $(DEPEND_FILE)

$(PROGRAMS): $(LIB_OBJECTS)

$(PROGRAMS): % : %.cpp
	@echo "Building program $@..."
	@$(CXX) $(LDFLAGS) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)%.o : %.cpp
	@echo "Building object $@..."
	@mkdir -p `dirname $@`
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

-include $(DEPEND_FILE)
