#include "import.hpp"

mdouble inverse_epsilon(mdouble energy) {
	return 0.0024 * energy + 0.68;
}

mdouble channel_to_energy(int channel) {
	return mdouble((1275.0 / 296 + 661.7 / 179) / 2, 0, (1275.0 / 296 - 661.7 / 179) / 2) * channel;
}

void run() {
	table result("result");
	result.add<int>("angle").add<mdouble>("sum");

	for (int angle = 25; angle <= 115; angle += 10) {
		table histo_al("al");
		histo_al.add<int>("channel").add<int>("count").read_file(str(format("compton_data/cs137-al-%1%.csv") % angle));

		table histo_bg("bg");
		histo_bg.add<int>("channel").add<int>("count").read_file(str(format("compton_data/cs137-none-%1%.csv") % angle));

		mdouble sum = 0;
		for (size_t i = 0; i < histo_al.rows(); ++i) {
			mdouble count = histo_al.at<int>("count", i) - (angle == 115 ? 1 : 2) * histo_bg.at<int>("count", i);
			count += variance(count.value());
			sum += inverse_epsilon(channel_to_energy(i)) * count;
		}

		result << angle << sum;
	}

	cout << result << endl;

	plot("plot <result> using 1:2:3 with errorbars");
}
