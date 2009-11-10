#ifndef _analyzer_measurement_hpp
#define _analyzer_measurement_hpp

#include <boost/operators.hpp>
#include <boost/call_traits.hpp>
#include <cmath>

namespace analyzer {

struct null_type {
};

template<class T, int i>
struct nth_parent {
	typedef typename nth_parent<typename T::super_type, i - 1>::type type;
};

template<class T>
struct nth_parent<T, 0> {
	typedef T type;
};

template<class T>
struct foreach_parent_impl {
	template<class F>
	foreach_parent_impl(T& value, F fn) {
		fn(value);
		foreach_parent_impl<typename T::super_type>(value, fn);
	}
};

template<>
struct foreach_parent_impl<null_type> {
	template<class F>
	foreach_parent_impl(null_type& value, F fn) {
	}
};

template<class T, class F>
void foreach_parent(T& value, F fn) {
	foreach_parent_impl<T>(value, fn);
}

template<class T, class S = null_type >
class gauss_uncertainty : public S {
public:

	typedef T                                          value_type;
	typedef S                                          super_type;
	typedef typename boost::call_traits<T>::param_type param_type;
	typedef gauss_uncertainty<T, S>                    this_type;

	gauss_uncertainty()
		: variance_(0) {
	}

	void add(param_type value, param_type other, const this_type& other_uncertainty) {
		variance_ += other_uncertainty.variance_;
	}

	void subtract(param_type value, param_type other, const this_type& other_uncertainty) {
		variance_ += other_uncertainty.variance_;
	}

	void multiply(param_type value, param_type other, const this_type& other_uncertainty) {
		variance_ = other * other * variance_ + value * value * other_uncertainty.variance_;
	}

	void divide(param_type value, param_type other, const this_type& other_uncertainty) {
               value_type tmp = value / (other * other);
	       variance_ = variance_ / (other * other) + tmp * tmp * other_uncertainty.variance_;
	}

	void sin(param_type value) {
		value_type t = std::cos(value);
		variance_ *= t * t;
	}

	void cos(param_type value) {
		value_type t = std::sin(value);
		variance_ *= t * t;
	}

	void tan(param_type value) {
		value_type t = std::tan(value);
		t = value_type(1) + t * t;
		variance_ *= t * t;
	}

	void sqrt(param_type value) {
		variance_ *= value_type(1) / (value_type(4) * value);
	}

	void log(param_type value) {
		value_type t = value_type(1) / value;
		variance_ *= t * t;
	}

	void exp(param_type value) {
		value_type t = std::exp(value);
		variance_ *= t * t;
	}

	const value_type& variance() const {
		return variance_;
	}

	value_type& variance() {
		return variance_;
	}

	value_type sigma() const {
		return std::sqrt(variance_);
	}

private:
	value_type variance_;
};

template<class T, class S = null_type >
class absolute_uncertainty : public S {
public:

	typedef T                                          value_type;
	typedef S                                          super_type;
	typedef typename boost::call_traits<T>::param_type param_type;
	typedef absolute_uncertainty<T, S>                 this_type;

	absolute_uncertainty()
		: absolute_error_(0) {
	}

	void add(param_type value, param_type other, const this_type& other_uncertainty) {
		absolute_error_ += other_uncertainty.absolute_error_;
	}

	void subtract(param_type value, param_type other, const this_type& other_uncertainty) {
		absolute_error_ += other_uncertainty.absolute_error_;
	}

	void multiply(param_type value, param_type other, const this_type& other_uncertainty) {
		absolute_error_ = std::abs(other * absolute_error_ + value * other_uncertainty.absolute_error_);
	}

	void divide(param_type value, param_type other, const this_type& other_uncertainty) {
	       absolute_error_ = std::abs(absolute_error_ / other + other_uncertainty.absolute_error_ * value / (other * other));
	}

	void sin(param_type value) {
		absolute_error_ *= std::abs(std::cos(value));
	}

	void cos(param_type value) {
		absolute_error_ *= std::abs(std::sin(value));
	}

	void tan(param_type value) {
		value_type t = std::tan(value);
		absolute_error_ *= std::abs(value_type(1) + t * t);
	}

	void sqrt(param_type value) {
		absolute_error /= value_type(2) * std::sqrt(value);
	}

	void log(param_type value) {
		absolute_error_ /= std::abs(value);
	}

	void exp(param_type value) {
		absolute_error *= std::exp(value);
	}

	const value_type& absolute_error() const {
		return absolute_error_;
	}

	value_type& absolute_error() {
		return absolute_error_;
	}

private:
	value_type absolute_error_;
};

#define UNCERTAINTY_BINARY_OP(op) \
template<class T> \
struct uncertainty_##op { \
	uncertainty_##op(const typename T::value_type& value, const typename T::value_type& other, const T& other_uncertainty) \
	: value_(value), other_(other), other_uncertainty_(other_uncertainty) {	\
	} \
	template<class U> \
	void operator()(U& uncertainty) { \
		uncertainty.op(value_, other_, other_uncertainty_); \
	} \
private: \
	const typename T::value_type& value_;   \
	const typename T::value_type& other_;   \
        const T& other_uncertainty_; \
};

#define UNCERTAINTY_UNARY_OP(op) \
template<class T> \
struct uncertainty_##op { \
	uncertainty_##op(const typename T::value_type& value) \
	: value_(value) {	\
	} \
	template<class U> \
	void operator()(U& uncertainty) { \
		uncertainty.op(value_); \
	} \
private: \
	const typename T::value_type& value_;   \
};

UNCERTAINTY_BINARY_OP(add)
UNCERTAINTY_BINARY_OP(subtract)
UNCERTAINTY_BINARY_OP(multiply)
UNCERTAINTY_BINARY_OP(divide)
UNCERTAINTY_UNARY_OP(sin)
UNCERTAINTY_UNARY_OP(cos)
UNCERTAINTY_UNARY_OP(tan)
UNCERTAINTY_UNARY_OP(sqrt)
UNCERTAINTY_UNARY_OP(log)
UNCERTAINTY_UNARY_OP(exp)

#undef UNCERTAINTY_BINARY_OP
#undef UNCERTAINTY_UNARY_OP

template<class T, class U>
class measurement : boost::additive<measurement<T, U>,
		    boost::additive2<measurement<T, U>, T,
		    boost::multiplicative<measurement<T, U>,
		    boost::multiplicative2<measurement<T, U>, T,
		    boost::equality_comparable<measurement<T, U>,
		    boost::less_than_comparable<measurement<T, U>,
		    boost::less_than_comparable2<measurement<T, U>, T > > > > > > > {
public:
	typedef T                                          value_type;
	typedef measurement<T, U>                          this_type;
	typedef U                                          uncertainty_type;
	typedef typename boost::call_traits<T>::param_type param_type;

	measurement(param_type value = value_type())
		: value_(value) {
	}

	measurement(const this_type& source)
		: value_(source.value_), uncertainty_(source.uncertainty_) {
	}

	this_type& operator=(const this_type& source) {
		if (this != &source) {
			value_ = source.value_;
			uncertainty_ = source.uncertainty_;
		}
		return *this;
	}

	this_type operator+() const {
		return *this;
	}

	this_type operator-() const {
		return this_type(-value_, uncertainty_);
	}

	this_type& operator+=(const this_type& other) {
		foreach_parent(uncertainty_, uncertainty_add<uncertainty_type>(value_, other.value_, other.uncertainty_));
		value_ += other.value_;
		return *this;
	}

	this_type& operator-=(const this_type& other) {
		foreach_parent(uncertainty_, uncertainty_subtract<uncertainty_type>(value_, other.value_, other.uncertainty_));
		value_ -= other.value_;
		return *this;
	}

	this_type& operator*=(const this_type& other) {
		foreach_parent(uncertainty_, uncertainty_multiply<uncertainty_type>(value_, other.value_, other.uncertainty_));
		value_  *= other.value_;
		return *this;
	}

	this_type& operator/=(const this_type& other) {
		foreach_parent(uncertainty_, uncertainty_divide<uncertainty_type>(value_, other.value_, other.uncertainty_));
		value_ /= other.value_;
		return *this;
	}

	bool operator<(const this_type& other) const {
		return value_ < other.value_;
	}

	bool operator<(param_type other) const {
		return value_ < other;
	}

	bool operator>(param_type other) const {
		return value_ > other;
	}

	const value_type& value() const {
		return value_;
	}

	value_type& value() {
		return value_;
	}

	template<int i>
	const typename nth_parent<uncertainty_type, i>::type& uncertainty() const {
		return uncertainty_;
	}

	template<int i>
	typename nth_parent<uncertainty_type, i>::type& uncertainty() {
		return uncertainty_;
	}

	const uncertainty_type& uncertainty() const {
		return uncertainty_;
	}

	uncertainty_type& uncertainty() {
		return uncertainty_;
	}

	///////////// HELPER METHODS //////////////

	measurement(param_type value, param_type variance_, param_type absolute_error_ = value_type())
		: value_(value) {
		variance() = variance_;
		absolute_error() = absolute_error_;
	}

	value_type& variance() {
		return uncertainty<0>().variance();
	}

	const value_type& variance() const {
		return uncertainty<0>().variance();
	}

	value_type& absolute_error() {
		return uncertainty<1>().absolute_error();
	}

	const value_type& absolute_error() const {
		return uncertainty<1>().absolute_error();
	}

	value_type sigma() const {
		return uncertainty<0>().sigma();
	}

	value_type total_error() const {
		return sigma() + absolute_error();
	}

	value_type absolute_error_percent() const {
		return 100 * absolute_error() / value();
	}

	value_type sigma_percent() const {
		return 100 * sigma() / value();
	}

	value_type total_error_percent() const {
		return 100 * total_error() /value();
	}

	value_type lower() const {
		return value() - sigma();
	}

	value_type upper() const {
		return value() + sigma();
	}

private:
	value_type       value_;
	uncertainty_type uncertainty_;
};

template<class T, class U>
measurement<T, U> sin(const measurement<T, U>& value) {
	measurement<T, U> result(std::sin(value.value()), value.uncertainty());
	foreach_parent(result.uncertainty(), uncertainty_sin<U>(value.value()));
	return result;
}

template<class T, class U>
measurement<T, U> cos(const measurement<T, U>& value) {
	measurement<T, U> result(std::cos(value.value()), value.uncertainty());
	foreach_parent(result.uncertainty(), uncertainty_cos<U>(value.value()));
	return result;
}

template<class T, class U>
measurement<T, U> tan(const measurement<T, U>& value) {
	measurement<T, U> result(std::tan(value.value()), value.uncertainty());
	foreach_parent(result.uncertainty(), uncertainty_tan<U>(value.value()));
	return result;
}

template<class T, class U>
measurement<T, U> sqrt(const measurement<T, U>& value) {
	measurement<T, U> result(std::sqrt(value.value()), value.uncertainty());
	foreach_parent(result.uncertainty(), uncertainty_sqrt<U>(value.value()));
	return result;
}

template<class T, class U>
measurement<T, U> log(const measurement<T, U>& value) {
	measurement<T, U> result(std::log(value.value()), value.uncertainty());
	foreach_parent(result.uncertainty(), uncertainty_log<U>(value.value()));
	return result;
}

template<class T, class U>
measurement<T, U> exp(const measurement<T, U>& value) {
	measurement<T, U> result(std::exp(value.value()), value.uncertainty());
	foreach_parent(result.uncertainty(), uncertainty_exp<U>(value.value()));
	return result;
}

template<class T>
struct simple_measurement {
	typedef measurement<T, gauss_uncertainty<T, absolute_uncertainty<T> > > type;
};

typedef simple_measurement<float>::type mfloat;
typedef simple_measurement<double>::type mdouble;

template<class T>
inline typename simple_measurement<T>::type variance(const T& x) {
	return typename simple_measurement<T>::type(0, x, 0);
}

template<class T>
inline typename simple_measurement<T>::type sigma(const T& x) {
	return typename simple_measurement<T>::type(0, x*x, 0);
}

template<class T>
inline typename simple_measurement<T>::type absolute_error(const T& x) {
	return typename simple_measurement<T>::type(0, 0, x);
}

} // namespace analyzer

#endif
