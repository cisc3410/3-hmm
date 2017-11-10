#include <assert.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "hmm.h"

HMM loadHMMFromFile(const std::string& fname, HMM::Observations& observed) {

	std::string line, str;
	float f;
	std::ifstream probsfile(fname);

	HMM::Distribution pprobs;
	std::map<std::string, HMM::Distribution> oprobs, tprobs;
	std::vector<std::string> states, observations;
	
	// load possible states and observations
	std::getline(probsfile, line);
	std::istringstream iss(line);
	while (iss >> str) states.push_back(str);
	std::getline(probsfile, line);
	std::istringstream iiss(line);
	while (iiss >> str) observations.push_back(str);

	// load prior probabilities
	std::getline(probsfile, line);
	std::getline(probsfile, line);
	assert(line == "# PRIOR");
	std::getline(probsfile, line);
	std::istringstream iiiss(line);
	int i = 0;
	while (iiiss >> f) pprobs[states[i++]] = f;

	// load observation probabilities
	std::getline(probsfile, line);
	std::getline(probsfile, line);
	assert(line == "# OBSERVATION");
	i = 0;
	while (!line.empty()) {
		int j = 0;
		std::getline(probsfile, line);
		std::istringstream ivss(line);
		while (ivss >> f) oprobs[observations[i]][states[j++]] = f;
		i++;
	}

	// load transition probabilities
	std::getline(probsfile, line);
	assert(line == "# TRANSITION");
	i = 0;
	while (!line.empty()) {
		int j = 0;
		std::getline(probsfile, line);
		std::istringstream ivss(line);
		while (ivss >> f) tprobs[states[i]][states[j++]] = f;
		i++;
	}

	// load observations
	std::getline(probsfile, line);
	assert(line == "# OBSERVATIONS");
	std::getline(probsfile, line);
	std::istringstream ivss(line);
	while (ivss >> str) observed.push_back(str);

	return HMM(pprobs, oprobs, tprobs);
}

int main(int argc, const char** argv) {
	if (argc != 2) {
		std::cout << "USAGE: ./a.out PROBLEMNAME\n";
		return 1;
	}
	std::string fname = argv[1] + std::string(".data");
	HMM::Observations observed;
	HMM hmm = loadHMMFromFile(fname, observed);

	std::cout << hmm.filter(observed.begin(), observed.end()) << std::endl;

	std::vector<std::string> mls = hmm.viterbi(observed.begin(), observed.end());
	std::cout << mls << std::endl;
}
