#ifndef _analyzer_plot_hpp
#define _analyzer_plot_hpp

#include <string>

namespace analyzer {

class plot_data;

class plot {
public:
	plot();
	explicit plot(const std::string&);
	~plot();

	plot& operator<<(const std::string&);

	plot& operator<<(plot& (*fn)(plot&)) {
		fn(*this);
		return *this;
	}

	static plot& endl(plot&);

private:
	void init();

	plot_data* data_;
};

}

#endif
