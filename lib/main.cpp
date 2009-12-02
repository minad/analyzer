#include "analyzer.hpp"
#include <boost/algorithm/string/trim.hpp>
#include <stdexcept>
#include <iostream>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/format.hpp>
#include <libgen.h>

void run();

namespace analyzer {

string_map cmdline;

std::string dirname(const std::string& name) {
	return ::dirname(const_cast<char*>(name.c_str()));
}

std::string basename(const std::string& name) {
	return ::basename(const_cast<char*>(name.c_str()));
}

}

int main(int argc, char* argv[]) {
	using namespace std;
	using boost::algorithm::trim_copy;
#ifdef NDEBUG
	try {
#endif
		FILE* fp = popen(boost::str(boost::format("make -C '%1%' %2% 2>&1") % analyzer::dirname(argv[0]) % analyzer::basename(argv[0])).c_str(), "r");
		boost::iostreams::stream<boost::iostreams::file_descriptor_source> in(fileno(fp));
		bool restart = false;
		while (in) {
			char buffer[1024];
			in.getline(buffer, sizeof(buffer) - 1);
			std::string line(buffer);
			if (line.find("Building") != std::string::npos)
				restart = true;
			if (line.find("make") != 0 && line.length() > 1)
				std::cout << line << std::endl;
		}

		if (restart) {
			std::cout << "Restarting " << argv[0] << std::endl;
			return execv(argv[0], argv);
		}

		cout << analyzer::pretty;
		cerr << analyzer::pretty;
		for (int i = 1; i < argc; ++i) {
			string s(argv[i]);
			string::size_type n = s.find('=');
			if (n != string::npos) {
				analyzer::cmdline[trim_copy(s.substr(0, n))] = trim_copy(s.substr(n + 1));
			}
		}
		run();
		return 0;
#ifdef NDEBUG
	} catch (const exception& ex) {
		std::cerr << ex.what() << std::endl;
		return 1;
	}
#endif
}
