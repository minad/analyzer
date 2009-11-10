#include "table.hpp"
#include "io.hpp"
#include <boost/foreach.hpp>
#include <boost/io/ios_state.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <iostream>
#include <sstream>

namespace analyzer {

table::table(const std::string& name)
	: object(name) {
}

void table::erase(size_type i) {
	names_.erase(columns_.at(i).name());
	columns_.erase(columns_.begin() + i);
}

table& table::add(const column_variant& col) {
	if (names_.find(col.name()) != names_.end())
		throw std::runtime_error("column name exists already");
	columns_.push_back(col);
	names_[col.name()] = columns_.size() - 1;
	return *this;
}

column_variant& table::column(const std::string& name) {
	name_map::const_iterator i = names_.find(name);
	if (i == names_.end())
		throw not_found();
	return columns_[i->second];
}

const column_variant& table::column(const std::string& name) const {
	name_map::const_iterator i = names_.find(name);
	if (i == names_.end())
		throw not_found();
	return columns_[i->second];
}

void table::write(std::ostream& out) const {
	output_mode mode = get_mode(out);
	if (mode == CSV) {
		using namespace boost::posix_time;
		ptime now = second_clock::local_time();
		out << "# Table:   " << name() << "\n"
			"# Date:    " << now << '\n';
		if (columns() > 0) {
			out << "# Columns: ";
			for (size_t i = 0; i < columns(); ++i) {
				if (i > 0) out << " | ";
				out << column(i).name();
				if (column(i).type() == static_typeid<mdouble>::id || column(i).type() == static_typeid<mfloat>::id)
					out << " | " << column(i).name() << " sigma | " << column(i).name() << " abs";
			}
			out << '\n';
			for (size_t row = 0; row < rows(); ++row) {
				out << at(0, row);
				for (size_t col = 1; col < columns(); ++col)
					out << ' ' << at(col, row);
				out << "\n";
			}
		}
	} else {
		std::vector<std::string> data(rows() * columns());
		std::vector<size_t> width(columns());
		std::ostringstream sout;
		set_mode(sout, mode);
		set_mformat(sout, get_mformat(out));
		for (size_t col = 0; col < columns(); ++col) {
			width[col] = column(col).name().length();

			for (size_t row = 0; row < rows(); ++row) {
				if (mode == LATEX &&
				    (column(col).type() == static_typeid<int>::id ||
				     column(col).type() == static_typeid<float>::id ||
				     column(col).type() == static_typeid<double>::id))
					sout << "\\num{" << at(col, row) << '}';
				else
					sout << at(col, row);
				size_t i = row * columns() + col;
				data[i] = sout.str();
				size_t len = data[i].size();
				if (len > width[col])
					width[col] = len;
				sout.str("");
			}
		}

		boost::io::ios_all_saver saver(out);
		using std::setw;
		if (mode == LATEX) {
			out << std::left;

			out << "\\begin{table}[H]\n"
				"  \\caption{" << name() << "}\n";
			if (columns() > 0 && rows() > 0) {
				out << "  \\begin{tabular}{|";
				for (size_t col = 0; col < columns(); ++col)
					out << "r|";
				out << "}\n    \\hline\n    ";
				for (size_t col = 0; col < columns(); ++col) {
					if (col > 0) out << " & ";
					out << setw(width[col]) << column(col).name() << setw(0);
				}
				out << "\\\\\n    \\hline\\hline\n";
				for (size_t row = 0; row < rows(); ++row) {
					out << "    ";
					for (size_t col = 0; col < columns(); ++col) {
						if (col > 0) out << " & ";
						out << setw(width[col]) << data[row * columns() + col] << setw(0);
					}
					out << "\\\\\n    \\hline\n";
				}
				out << "  \\end{tabular}\n";
			}
			out << "\\end{table}\n";
		} else {
			out << "Table: " << name() << '\n';
			if (columns() > 0 && rows() > 0) {
				out << std::left;
				for (size_t col = 0; col < columns(); ++col)
					out << "| " << setw(width[col]) << column(col).name() << setw(0) << ' ';
				out << "|\n" << std::right;
				for (size_t row = 0; row < rows(); ++row) {
					for (size_t col = 0; col < columns(); ++col)
						out << "| " << setw(width[col]) << data[row * columns() + col] << setw(0) << ' ';
					out << "|\n";
				}
			}
		}
	}
}

template<class T>
inline void read_field(std::istream& in, column_variant& col) {
	if (static_typeid<T>::id == col.type()) {
		T value;
		in >> value;
		col.as<T>() << value;
	}
}

// Read CSV format
void table::read(std::istream& stream) {
	char buffer[0xFFFF];
	while (stream.good()) {
		stream.getline(buffer, sizeof (buffer));
		std::string line(buffer);
		boost::trim(line);
		if (line.empty() || line[0] == '#')
			continue;
		std::istringstream in(line);
		BOOST_FOREACH (column_variant& col,  columns_) {
			read_field<int>(in, col);
			read_field<float>(in, col);
			read_field<double>(in, col);
			read_field<mfloat>(in, col);
			read_field<mdouble>(in, col);
			read_field<std::string>(in, col);
		}
	}
}

} // namespace analyzer
