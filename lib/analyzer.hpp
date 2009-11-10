#ifndef _analyzer_analyzer_hpp
#define _analyzer_analyzer_hpp

#include <map>
#include <string>
#include "measurement.hpp"
#include "table.hpp"
#include "fit.hpp"
#include "plot.hpp"
#include "io.hpp"
#include "algorithm.hpp"

namespace analyzer {
typedef std::map<std::string, std::string> string_map;
extern string_map cmdline;
}

#endif
