#ifndef _analyzer_algorithm_h
#define _analyzer_algorithm_h

#include "measurement.hpp"
#include <algorithm>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

namespace analyzer {
namespace algorithm {

using namespace boost::lambda;

template<class I, class F>
typename I::value_type sum(I first, I last, F f) {
	typename I::value_type result = typename I::value_type();
	std::for_each(first, last, result += bind(f, _1)(_1));
	return result;
}

template<class I>
inline typename I::value_type sum(I first, I last) {
	return sum(first, last, _1);
}

template<class T, class F>
inline typename T::value_type sum(const T& x, F f) {
	return sum(x.begin(), x.end(), f);
}

template<class T>
inline typename T::value_type sum(const T& x) {
	return sum(x, _1);
}

template<class I>
typename I::value_type standard_deviation(I first, I last) {
	typedef typename I::value_type V;
	V N = last - first;
	V mean = sum(first, last) / N;
	V square_sum = sum(first, last, _1 * _1);
	return std::sqrt(square_sum / (N - 1) + mean * mean);
}

template<class T>
inline typename T::value_type standard_deviation(const T& x) {
	return standard_deviation(x.begin(), x.end());
}

template<class I>
typename simple_measurement<typename I::value_type>::type mean(I first, I last) {
	typedef typename I::value_type V;
	V N = last - first;
	V mean = sum(first, last) / N;
	V square_sum = sum(first, last, _1 * _1);
	return typename simple_measurement<V>::type(mean, std::sqrt(square_sum / (N * (N - 1)) + mean * mean));
}

template<class T>
typename simple_measurement<typename T::value_type>::type mean(const T& x) {
	return mean(x.begin(), x.end());
}

}
}

#endif
