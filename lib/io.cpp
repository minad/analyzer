#include "measurement.hpp"
#include "io.hpp"
#include "object.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <boost/io/ios_state.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace analyzer {

namespace {
	const char* DEFAULT_FORMAT = "%m %p %s %p %a";

	int mformat_index(std::ostream& out) {
		static int index = std::ostream::xalloc();
		return index;
	}

	long& mode_flag(std::ostream& out) {
		static int index = std::ostream::xalloc();
		return out.iword(index);
	}
}

void set_mode(std::ostream& out, output_mode mode) {
	mode_flag(out) = mode;
}

output_mode get_mode(std::ostream& out) {
	long mode = mode_flag(out);
	return *reinterpret_cast<output_mode*>(&mode);
}

const char* get_mformat(std::ostream& out) {
	const char* format = static_cast<const char*>(out.pword(mformat_index(out)));
	return format ? format : DEFAULT_FORMAT;
}

void set_mformat(std::ostream& out, const char* format) {
	out.pword(mformat_index(out)) = const_cast<char*>(format);
}

std::ostream& operator<<(std::ostream& out, const mformat& format) {
	set_mformat(out, format.format);
	return out;
}

std::ostream& default_mformat(std::ostream& out) {
	return out << mformat(DEFAULT_FORMAT);
}

template<class T, class U>
std::istream& operator>>(std::istream& in, measurement<T, U>& m) {
	T sigma;
	in >> m.value() >> sigma >> m.absolute_error();
	m.variance() = sigma * sigma;
	return in;
}

template<class T, class U>
std::ostream& operator<<(std::ostream& out, const measurement<T, U>& m) {
	using namespace boost;
	output_mode mode = get_mode(out);
	if (mode == CSV) {
		boost::io::ios_precision_saver saver(out);
		out << std::setprecision(15) << m.value() << ' ' << m.sigma() << ' ' << m.absolute_error();
	} else {
		std::string prefix, suffix, op;
		if (mode == LATEX) {
			prefix = "\\num{";
			suffix = "}";
			op = "\\pm";
		} else {
			op = "+/-";
		}

		std::string str = get_mformat(out);
		replace_all(str, "%p", op);
		replace_all(str, "%m", prefix + tostr(m.value()) + suffix);
		replace_all(str, "%v", prefix + tostr(m.variance()) + suffix);
		replace_all(str, "%s", prefix + tostr(m.sigma()) + suffix);
		replace_all(str, "%a", prefix + tostr(m.absolute_error()) + suffix);
		replace_all(str, "%t", prefix + tostr(m.total_error()) + suffix);
		replace_all(str, "%S", prefix + tostr(m.sigma_percent()) + suffix);
		replace_all(str, "%A", prefix + tostr(m.absolute_error_percent()) + suffix);
		replace_all(str, "%T", prefix + tostr(m.total_error_percent()) + suffix);
		out << str;
	}
	return out;
}

std::istream& operator>>(std::istream& in, object& object) {
	object.read(in);
	return in;
}

std::ostream& operator<<(std::ostream& out, const object& object) {
	object.write(out);
	return out;
}

namespace {
	void instantiate() {
		mfloat f;
		mdouble d;
		std::cout << f << d;
		std::cin >> f >> d;
	}
}

}
