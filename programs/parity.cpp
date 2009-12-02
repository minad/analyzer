#include "import.hpp"

table alle_werte("alle_werte");

void compute(const string& name) {
	mdouble phi = 0.52 + sigma(0.05);
	double f = 2./26;

	table werte("werte");
	werte.add<int>(name + " 1").add<int>(name + " 2");
	werte.read_file("parity_data/" + name);

	cout << mformat("%m %p %s (%S %)");

	cout << name << endl;

	int sx = algorithm::sum(werte.column<int>(0).begin(), werte.column<int>(0).end());
	int sy = algorithm::sum(werte.column<int>(1).begin(), werte.column<int>(1).end());
	mdouble nx(sx, sx);
	mdouble ny(sy, sy);

	mdouble E = (nx-ny)/(nx+ny);
	cout << "E (summe) = " << E << endl;
	cout << "P_C = " << E / (f * phi) << endl;

	table abweichungen("abweichung");
	abweichungen.add<double>(name + " abweichung");
	for (size_t i = 0; i < werte.rows(); ++i) {
	  int nx = werte.at<int>(0, i);
	  int ny = werte.at<int>(1, i);
	  abweichungen << double(nx-ny)/double(nx+ny);
	}

	mdouble mean = algorithm::mean(abweichungen.column<double>(0).begin(), abweichungen.column<double>(0).end());
	E = mdouble(mean.value(), mean.variance() / werte.rows());
	cout << "E (gemittelt) = " << E << endl;
	cout << "P_C = " << E / (f * phi) << endl;

	alle_werte.add(werte.column(0)).add(werte.column(1)).add(abweichungen.column(0));
}

void run() {
	compute("werte.csv");
	compute("werte2.csv");
	cout << default_mformat << alle_werte << endl;
}
