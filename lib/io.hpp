#ifndef _analyzer_io_hpp
#define _analyzer_io_hpp

#include <sstream>

namespace analyzer {

class object;

enum output_mode {
	CSV,
	LATEX,
	PRETTY,
};

output_mode get_mode(std::ostream& out);
void set_mode(std::ostream& out, output_mode mode);

// default mode
inline std::ostream& csv(std::ostream& out) {
	set_mode(out, CSV);
	return out;
}

inline std::ostream& latex(std::ostream& out) {
	set_mode(out, LATEX);
	return out;
}

inline std::ostream& pretty(std::ostream& out) {
	set_mode(out, PRETTY);
	return out;
}

std::istream& operator>>(std::istream&, object&); // can only read csv format
std::ostream& operator<<(std::ostream&, const object&);

template<class T, class U>
std::istream& operator>>(std::istream&, measurement<T, U>&); // can only read csv format

template<class T, class U>
std::ostream& operator<<(std::ostream&, const measurement<T, U>&);

/*
 * Measurement format applies only to pretty/latex mode
 *
 * %p ... +/- or latex \pm
 * %m ... value
 * %v ... variance
 * %s ... sigma
 * %a ... absolute error
 * %t ... total error
 * %S ... sigma in percent
 * %A ... absolute error in percent
 * %T ... total error in percent
 */
struct mformat {
	const char* format;
	explicit mformat(const char* f) : format(f) {}
};

std::ostream& default_mformat(std::ostream&);
std::ostream& operator<<(std::ostream&, const mformat&);

const char* get_mformat(std::ostream& out);
void set_mformat(std::ostream& out, const char*);

template<class T>
std::string tostr(const T& v) {
	std::ostringstream out;
	out << v;
	return out.str();
}

}

#endif
