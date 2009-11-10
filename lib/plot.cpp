#include "plot.hpp"
#include "table.hpp"
#include "fit.hpp"
#include <stdexcept>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/regex.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <iostream>

namespace analyzer {

namespace {
	boost::regex regex("<([^>]+)>");
	int file_count = 0;
}

struct plot_data {
	plot_data(FILE* p)
		: pipe(p), stream(fileno(p)) {
	}

	FILE* pipe;
	boost::iostreams::stream<boost::iostreams::file_descriptor_sink> stream;
};

plot::plot() {
	init();
}

plot::plot(const std::string& cmd) {
	init();
	*this << cmd << endl;
}

plot::~plot() {
	data_->stream.close();
	fclose(data_->pipe);
	delete data_;
}

void plot::init() {
	FILE* fp = popen("gnuplot -persist", "w");
	if (!fp)
		throw std::runtime_error("could not start gnuplot");
	data_ = new plot_data(fp);
}

plot& plot::operator<<(const std::string& cmd) {
	std::string str = cmd;

	boost::smatch match;
	while (regex_search(str, match, regex)) {
		object* obj = registry::get(match[1]);

		table* tab = dynamic_cast<table*>(obj);
		if (tab) {
			std::string path = boost::str(boost::format("/tmp/plot%1%.%2%.csv") % getuid() % file_count++);
			str.replace(match.position(), match.length(), '"' + path + '"');
			tab->write_file(path);
			continue;
		}

		expression* exp = dynamic_cast<expression*>(obj);
		if (exp) {
			str.replace(match.position(), match.length(), exp->as_string());
			continue;
		}

		throw std::runtime_error("unknown type");
	}

	data_->stream << str;
	return *this;
}

plot& plot::endl(plot& p) {
	p.data_->stream << std::endl;
	return p;
}

}
