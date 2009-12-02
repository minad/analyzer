#include "import.hpp"
#include <fstream>

void run() {
	string file = cmdline["file"];
	if (file.length() == 0) {
		cerr << "file argument required" << endl;
		return;
	}

	table tab("messwerte");
	tab.add<double>("U").add<double>("deltaT").add<double>("sigmaT");
	tab.read_file(file);

	double deltaX = 23.5;
	double sigmaX = 0.5;
	double P = 1000;
	double sigmaP = 10;
	string abstand = file.substr(file.length() - 6).substr(0, 2);
	if (abstand == "36")
		deltaX = 36.3;
	else if (abstand == "07")
		P = 700;
	else if (abstand == "13")
		P = 1300;

	cout << "deltaX = " << deltaX << endl;
	cout << "P = " << P << endl;

	tab.add<mdouble>("v").add<mdouble>("Ep");
	for (size_t y = 0; y < tab.rows(); ++y) {
		double sigmaU=10;
		double sigmaD=0.4;
		double deltaT = tab.at<double>("deltaT", y);
		double sigmaT = tab.at<double>("sigmaT", y);
		double U = tab.at<double>("U",y)*1000;
		double tmp = deltaX / (deltaT*deltaT);
		tab.column<mdouble>("v") << deltaX / deltaT + variance(1.0/(deltaT*deltaT)*sigmaX*sigmaX + tmp * tmp * sigmaT * sigmaT);
		double E=U/40;
		double sigmaE=sqrt((1.0/40*sigmaU)*(1.0/40*sigmaU) + (U/(1600.0)*sigmaD)*(U/(1600.0)*sigmaD));
		tab.column<mdouble>("Ep") << E/P + variance(1/(P*P)*sigmaE*sigmaE + (E*E/(P*P*P*P))*sigmaP*sigmaP);
	}

	cout << mformat("%m %p %s (%S)");

	cout << tab << endl;
	cout << latex << tab <<endl;
	tab.write_file(file.substr(0, file.length() - 4) + "-result.csv");

	ofstream out("drift.tex", ofstream::app);
	out << mformat("%m %p %s (%S)");
	out << file << endl;
	out << latex << tab << endl;

	plot("plot <messwerte> using 1:2 title 'deltaT/U'");
	plot("plot <messwerte> using 1:4 title 'v/U'");
	plot("plot <messwerte> using 7:4 title 'v/Ep'");
}
