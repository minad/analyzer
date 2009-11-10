#include "import.hpp"

void run() {
	variate_generator<mt19937, normal_distribution<> > rand(mt19937(), normal_distribution<>(0, 10));

	table table("table");
	table.add<double>("x").add<double>("y");
	for (int i = 0; i < 200; ++i)
		table << double(i) << double(i) + rand();

	table.write_file("test.csv");

	linear_fit fit("fn", table);
	cout << fit << endl;
	plot("plot <table>, <fn>");
}
