#include "analyzer.hpp"
#include <boost/algorithm/string/trim.hpp>
#include <stdexcept>
#include <iostream>

void run();

namespace analyzer {

string_map cmdline;

}

int main(int argc, char* argv[]) {
	using namespace std;
	using boost::algorithm::trim_copy;
#ifdef NDEBUG
	try {
#endif
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
