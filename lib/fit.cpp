#include "fit.hpp"
#include "io.hpp"
#include <iostream>
#include <boost/format.hpp>
#include <gsl/gsl_fit.h>

namespace analyzer {

linear_fit::linear_fit(const std::string& name, const table& table, size_t x, size_t y)
	: object(name) {
	compute(table.column<double>(x), table.column<double>(y));
}

linear_fit::linear_fit(const std::string& name, const table& table, const std::string& x, const std::string& y)
	: object(name) {
	compute(table.column<double>(x), table.column<double>(y));
}

mdouble linear_fit::operator()(double x) const {
	return m_ * x + y0_ + variance(2 * x * cov_);
}

void linear_fit::compute(const column<double>& x, const column<double>& y) {
	std::vector<double> data(x);
	data.insert(data.end(), y.begin(), y.end());

	double c0, c1, cov00, cov01, cov11, sumsq;
	gsl_fit_linear(&(*data.begin()), 1, &(*(data.begin() + x.size())), 1, x.size(), &c0, &c1, &cov00, &cov01, &cov11, &sumsq);

	m_ = mdouble(c1, cov11);
	y0_ = mdouble(c0, cov00);
	cov_ = cov01;
}

std::string linear_fit::as_string() const {
	return boost::str(boost::format("%1% * x + %2%") % m_.value() % y0_.value());
}

void linear_fit::write(std::ostream& out) const {
	switch (get_mode(out)) {
	case PRETTY:
		out << name() << "(x) = (" << m_ << ") * x + (" << y0_ << ')';
		break;
	case LATEX:
		out << name() << "(x) = (" << m_ << ") \\cdot x + (" << y0_ << ')';
		break;
	default:
		out << m_ << ' ' << y0_ << ' ' << cov_;
		break;
	}
}

void linear_fit::read(std::istream& in) {
	in >> m_ >> y0_ >> cov_;
}

}
