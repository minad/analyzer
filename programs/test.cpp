#include "import.hpp"

void run() {
	//plot p;
	//p << "plot sin(x)" << plot::wait;

	variate_generator<mt19937, normal_distribution<> > rand(mt19937(), normal_distribution<>(0, 10));

	table table("table");
	table.add<double>("x").add<mdouble>("zof y").add<int>("z laber");
	for (int i = 0; i < 50; ++i)
		table << double(i) << mdouble(i) + rand() << i;

	cout << latex << table << pretty << endl;

	analyzer::table table2("table2");
	table2.add<double>("x");

	for (int i = 0; i < 50; ++i)
		table2 << rand();

	cout << algorithm::mean(table2.column<double>(0)) << endl;
}
