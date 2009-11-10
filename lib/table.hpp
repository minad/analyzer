#ifndef _analyzer_table_hpp
#define _analyzer_table_hpp

#include "measurement.hpp"
#include "object.hpp"
#include <boost/variant.hpp>
#include <boost/call_traits.hpp>
#include <vector>
#include <string>
#include <map>

namespace analyzer {

template<class T> struct static_typeid {};
#define STATIC_TYPEID(t, i) template<> struct static_typeid<t> { static const int id = i; static const char* name() { return #t; } };
STATIC_TYPEID(int, 0)
STATIC_TYPEID(float, 1)
STATIC_TYPEID(double, 2)
STATIC_TYPEID(mfloat, 3)
STATIC_TYPEID(mdouble, 4)
STATIC_TYPEID(std::string, 5)

template<class T>
class column : public std::vector<T> {
public:
	typedef T                                          value_type;
	typedef std::vector<value_type>                    super_type;
	typedef typename boost::call_traits<T>::param_type param_type;
	typedef typename super_type::size_type             size_type;

	column(const std::string& name)
		:  name_(name) {
	}

	const std::string& name() const {
		return name_;
	}

	column& operator<<(param_type value) {
		push_back(value);
		return *this;
	}

	size_type rows() const {
		return super_type::size();
	}

private:
	std::string name_;
};

class column_variant {
public:
	typedef size_t size_type;
	typedef boost::variant<int,
			       float,
			       double,
			       mfloat,
			       mdouble,
			       std::string> field_variant;

private:
	struct size_visitor : public boost::static_visitor<size_type> {
		template<class T>
		size_type operator()(const T& column) const {
			return column.size();
		}
	};

	struct name_visitor : public boost::static_visitor<const std::string&> {
		template<class T>
		const std::string& operator()(const T& column) const {
			return column.name();
		}
	};

	struct type_visitor : public boost::static_visitor<int> {
		template<class T>
		int operator()(const T& container) const {
			return static_typeid<typename T::value_type>::id;
		}
	};

	struct field_visitor : public boost::static_visitor<field_variant> {
		field_visitor(size_type index)
			: index_(index) {
		}
		template<class T>
		field_variant operator()(const T& column) const {
			return column.at(index_);
		}
	private:
		size_type index_;
	};

public:

	template<class T>
	column_variant(const T& data)
		: data_(data) {
	}

	size_type size() const {
		return apply_visitor(size_visitor(), data_);
	}

	size_type rows() const {
		return size();
	}

	int type() const {
		return apply_visitor(type_visitor(), data_);
	}

	const std::string& name() const {
		return apply_visitor(name_visitor(), data_);
	}

	field_variant at(size_type i) const {
		return apply_visitor(field_visitor(i), data_);

	}

	template<class T>
	column<T>& as() {
		return boost::get<column<T> >(data_);
	}

	template<class T>
	const column<T>& as() const {
		return boost::get<column<T> >(data_);
	}

	template<class T>
	T& at(size_type i) {
		return as<T>().at(i);
	}

	template<class T>
	const T& at(size_type i) const {
		return as<T>().at(i);
	}

private:

	boost::variant<column<int>,
		       column<float>,
		       column<double>,
		       column<mfloat>,
		       column<mdouble>,
		       column<std::string> > data_;
};

class table : public object {
public:
	typedef std::vector<column_variant> column_vector;
	typedef column_vector::size_type    size_type;

	table(const std::string&);

	size_type rows() const {
		return column(0).size();
	}

	size_type columns() const {
		return columns_.size();
	}

	column_variant& column(size_type i) {
		return columns_.at(i);
	}

	const column_variant& column(size_type i) const {
		return columns_.at(i);
	}

	column_variant& column(const std::string&);
	const column_variant& column(const std::string&) const;

	template<class T>
	analyzer::column<T>& column(size_type i) {
		return column(i).as<T>();
	}

	template<class T>
	const analyzer::column<T>& column(size_type i) const {
		return column(i).as<T>();
	}

	template<class T>
	analyzer::column<T>& column(const std::string& i) {
		return column(i).as<T>();
	}

	template<class T>
	const analyzer::column<T>& column(const std::string& i) const {
		return column(i).as<T>();
	}

	column_variant::field_variant at(size_type col, size_type row) const {
		return column(col).at(row);
	}

	column_variant::field_variant at(const std::string& col, size_type row) const {
		return column(col).at(row);
	}

	template<class T>
	T& at(size_type col, size_type row) {
		return column<T>(col).at(row);
	}

	template<class T>
	const T& at(size_type col, size_type row) const {
		return column<T>(col).at(row);
	}

	template<class T>
	T& at(const std::string& col, size_type row) {
		return column<T>(col).at(row);
	}

	template<class T>
	const T& at(const std::string& col, size_type row) const {
		return column<T>(col).at(row);
	}

	template<class T>
	void set(const std::string& col, size_type row, const T& value) const {
		column<T>(col).at(row) = value;
	}

	template<class T>
	void set(size_type col, size_type row, const T& value) const {
		column<T>(col).at(row) = value;
	}

	void erase(size_type);

	table& add(const column_variant&);

	template<class T>
	table& add(const std::string& name) {
		return add(analyzer::column<T>(name));
	}

	virtual void write(std::ostream&) const;
	virtual void read(std::istream&);

private:

	class filler {
	public:
		filler(table& table)
			: table_(table), index_(1) {
		}

		template<class T>
		filler& operator<<(const T& value) {
			table_.column<T>(index_++ % table_.columns()) << value;
			return *this;
		}

	private:
		table&           table_;
		table::size_type index_;
	};

public:
	template<class T>
	filler operator<<(const T& value) {
		column<T>(0) << value;
		return filler(*this);
	}

private:
	typedef std::map<std::string, size_t> name_map;
	name_map      names_;
	column_vector columns_;
};

} // namespace analyzer

#endif
