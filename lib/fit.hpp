#ifndef _analyzer_fit_hpp
#define _analyzer_fit_hpp

#include "table.hpp"

namespace analyzer {

struct expression {
	virtual ~expression() {}
	virtual std::string as_string() const = 0;
};

// y = m * x + y0
class linear_fit : public object, public expression {
public:
	linear_fit(const std::string&, const table&, size_t = 0, size_t = 1);
	linear_fit(const std::string&, const table&, const std::string&, const std::string&);

	mdouble operator()(double) const;

	mdouble m() const {
		return m_;
	}

	mdouble y0() const {
		return y0_;
	}

	double cov() const {
		return cov_;
	}

	virtual std::string as_string() const;

	virtual void write(std::ostream&) const;
	virtual void read(std::istream&);

private:
	void compute(const column<double>&, const column<double>&);

	mdouble m_, y0_;
	double  cov_;
};

}

#endif
