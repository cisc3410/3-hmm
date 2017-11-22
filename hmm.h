#ifndef HMM_H
#define HMM_H

#include <iostream>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "hmm.h"

typedef std::map<std::string, std::map<std::string, float>> StringStringFloatMap;

class HMM {

	public:
	typedef std::map<std::string, float> Distribution;
	typedef std::vector<std::string> Observations;

	HMM(const Distribution&, const std::map<std::string, Distribution>&, const std::map<std::string, Distribution>&);

	Distribution filter(Observations::iterator, Observations::iterator);
	std::vector<std::string> viterbi(Observations::iterator, Observations::iterator);

	private:
	// prior, observation, transition probabilities
	Distribution pprobs_;
	std::map<std::string, Distribution> oprobs_, tprobs_; 

	// list of states for convenience
	const std::vector<std::string> states_;	

	void normalize(Distribution&);

};

std::ostream& operator<<(std::ostream&, const HMM::Distribution&);
std::ostream& operator<<(std::ostream&, const std::map<std::string, HMM::Distribution>&);
std::ostream& operator<<(std::ostream&, const std::map<std::string, std::map<std::string, std::string>>&);
std::ostream& operator<<(std::ostream&, const std::vector<HMM::Distribution>&);
std::ostream& operator<<(std::ostream&, const std::vector<std::string>&);

#endif
