#include <iomanip>
#include <iostream>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "hmm.h"

template <class K, class V>
std::vector<std::string> keys(std::map<K, V> d) {
	std::vector<K> rv;
	for (auto item : d) rv.push_back(item.first);
	return rv;
}

std::ostream& operator<<(std::ostream& stream, const HMM::Distribution& dist) {
	for (auto item : dist) {
		stream << item.first << ": " << item.second << std::endl;
	}
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const std::vector<HMM::Distribution>& distv) {
	std::map<std::string, HMM::Distribution> distm;
	for (int i = 0; i < distv.size(); i++) {
		for (auto item : distv[i]) {
			distm[item.first][std::to_string(i+1)] = item.second;
		}
	}
	stream << distm;
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const std::map<std::string, HMM::Distribution>& dist) {
	std::vector<std::string> outerkeys = keys(dist), innerkeys = keys(dist.begin()->second);
	int okmw = 0, ikmw = 0;
	for (auto s : outerkeys) okmw = std::max(okmw, (int)s.length());
	for (auto s : innerkeys) ikmw = std::max(ikmw, (int)s.length());
	stream << std::string(okmw+1, ' ');
	for (auto is : innerkeys) {
		stream << " " << is;
	}
	stream << std::endl;
	for (auto os : outerkeys) {
		stream << std::setw(okmw) << os << ":";
		for (auto is : innerkeys) {
			stream << " " << std::setw(is.length()) << std::setprecision(2) << dist.find(os)->second.find(is)->second;
		}
		stream << std::endl;
	}
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const std::vector<std::string>& v) {
	for (auto s : v) {
		stream << s << " ";
	}
	return stream;
}

HMM::HMM(const Distribution& pprobs, const std::map<std::string, Distribution>& oprobs, const std::map<std::string, Distribution>& tprobs) : pprobs_(pprobs), oprobs_(oprobs), tprobs_(tprobs), states_(keys(tprobs)) {}

// normalize distribution by making all probabilities sum to 1
void HMM::normalize(Distribution& dist) {
	float sum = 0;
	for (auto item : dist) {
		sum += item.second;
	}
	if (!sum)	{
		std::cout << "Error: all probabilities are zero\n";
		throw std::exception();
	}
	for (auto& item : dist) {
		item.second /= sum;
	}
}

HMM::Distribution uniformDistribution(const std::vector<std::string>& ss) {
	HMM::Distribution d;
	for (auto s : ss) d[s] = 1;
	return d;
}

void operator*=(HMM::Distribution& lhs, const HMM::Distribution& rhs) {
	for (const auto item : rhs) {
		lhs[item.first] *= item.second;
	}
}

void operator+=(HMM::Distribution& lhs, const HMM::Distribution& rhs) {
	for (const auto item : rhs) {
		lhs[item.first] += item.second;
	}
}

HMM::Distribution operator+(const HMM::Distribution& lhs, const HMM::Distribution& rhs) {
	HMM::Distribution d;
	for (const auto item : lhs) {
		d[item.first] = item.second + rhs.find(item.first)->second;
	}
	return d;
}

HMM::Distribution operator*(const HMM::Distribution& lhs, const HMM::Distribution& rhs) {
	HMM::Distribution d;
	for (const auto item : lhs) {
		d[item.first] = item.second * rhs.find(item.first)->second;
	}
}

// TODO: Filtering: Given a list of T observations, return the 
// posterior probability distribution over the most recent state
// (Given the observations, what is the probability the
// most recent state has each of the possible values)
// (In this case, sunny, rainy, or foggy)
HMM::Distribution HMM::filter(Observations::iterator begin, Observations::iterator end) {
}


// TODO: Viterbi: Given a list of T observations, return the most
// likely sequence of states (e.g. { "sunny", "rainy", "foggy" ... }
std::vector<std::string> HMM::viterbi(Observations::iterator begin, Observations::iterator end) {

}
