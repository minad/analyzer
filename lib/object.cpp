#include "object.hpp"
#include <stdexcept>
#include <fstream>

namespace analyzer {

object::object(const std::string& name)
	: name_(name) {
	registry::get()->add(this);
}

object::~object() {
	registry::get()->remove(this);
}

void object::read_file(const std::string& file) {
	std::ifstream in(file.c_str());
	read(in);
}

void object::write_file(const std::string& file) const {
	std::ofstream out(file.c_str());
	write(out);
}

void registry::add(object* obj) {
	if (objects_.find(obj->name()) != objects_.end())
		throw std::runtime_error("object name conflict");
	objects_[obj->name()] = obj;
}

void registry::remove(object* obj) {
	object_map::iterator i = objects_.find(obj->name());
	if (i == objects_.end())
		throw not_found();
	objects_.erase(i);
}

object* registry::get(const std::string& name) {
	registry* instance = get();
	object_map::const_iterator i = instance->objects_.find(name);
	if (i == instance->objects_.end())
		throw not_found();
	return i->second;
}

registry* registry::get() {
	static registry* instance = new registry;
	return instance;
}

}
