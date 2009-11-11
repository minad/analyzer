#include "import.hpp"

table alle_werte("alle_werte");

void compute(const string& name) {
	table werte("werte");
	werte.add<int>(name + " 1").add<int>(name + " 2");
	werte.read_file("parity_data/" + name);

	cout << name << endl;

	int sx = algorithm::sum(werte.column<int>(0).begin(), werte.column<int>(0).end());
	int sy = algorithm::sum(werte.column<int>(1).begin(), werte.column<int>(1).end());
	mdouble nx(sx, sx);
	mdouble ny(sy, sy);

	cout << (nx-ny)/(nx+ny) << endl;

	table abweichungen("abweichung");
	abweichungen.add<double>(name + " abweichung");
	for (size_t i = 0; i < werte.rows(); ++i) {
	  int nx = werte.at<int>(0, i);
	  int ny = werte.at<int>(1, i);
	  abweichungen << double(nx-ny)/double(nx+ny);
	}

	cout << algorithm::mean(abweichungen.column<double>(0).begin(), abweichungen.column<double>(0).end()) << endl;

	alle_werte.add(werte.column(0)).add(werte.column(1)).add(abweichungen.column(0));
}

void run() {
	compute("werte.csv");
	compute("werte2.csv");
	cout << alle_werte << endl;
}
