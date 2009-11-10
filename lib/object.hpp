#ifndef _analyzer_object_hpp
#define _analyzer_object_hpp

#include <string>
#include <map>
#include <stdexcept>
#include <boost/noncopyable.hpp>

namespace analyzer {

struct not_found : std::exception {
	virtual const char * what() const throw() {
		return "object not found";
	}
};

class object : boost::noncopyable {
public:
	object(const std::string&);
	virtual ~object();

	const std::string& name() const {
		return name_;
	}

	virtual void write(std::ostream&) const = 0;
	virtual void read(std::istream&) = 0;

	void read_file(const std::string&);
	void write_file(const std::string&) const;

private:
	std::string name_;
};

class registry : boost::noncopyable {
public:

	static object* get(const std::string&);

private:
	friend class object;

	registry() {}

	static registry* get();

	void add(object*);
	void remove(object*);

	typedef std::map<std::string, object*> object_map;
	object_map objects_;
};

}

#endif
