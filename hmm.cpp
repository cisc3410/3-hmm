#include <iomanip>
#include <iostream>
#include <fstream>
#include <limits>
#include <map>
#include <math.h>
#include <sstream>
#include <string>
#include <vector>

#include "hmm.h"

#define INF std::numeric_limits<float>::max()

template <class K, class V>
std::vector<std::string> keys(std::map<K, V> d) {
	std::vector<K> rv;
	for (auto item : d) rv.push_back(item.first);
	return rv;
}

std::ostream& operator<<(std::ostream& stream, const HMM::Distribution& dist) {
	for (auto item : dist) {
		stream << item.first << ": " << std::fixed << std::setprecision(2) << item.second << std::endl;
	}
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const std::vector<HMM::Distribution>& distv) {
	std::map<std::string, std::map<std::string, std::string>> distm;
	for (int i = 0; i < distv.size(); i++) {
		for (auto item : distv[i]) {
			std::stringstream ss;
			ss << std::fixed << std::setprecision(2) << item.second;
			distm[item.first][std::to_string(i)] = item.second == -INF ? "-INF" : ss.str();
		}
	}
	stream << distm;
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const std::vector<std::map<std::string, std::string>>& vm) {
	std::map<std::string, std::map<std::string, std::string>> mm;
	for (int i = 0; i < vm.size(); i++) {
		for (auto item : vm[i]) {
			mm[item.first][std::to_string(i)] = item.second;
		}
	}
	stream << mm;
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const std::map<std::string, HMM::Distribution>& dist) {
	std::map<std::string, std::map<std::string, std::string>> m;
	for (auto item : dist) {
		for (auto itemi : item.second) {
			std::stringstream ss;
			ss << std::fixed << std::setprecision(2) << itemi.second;
			m[item.first][itemi.first] = ss.str();
		}
	}
	stream << m;
	return stream;
}

std::ostream& operator<<(std::ostream& stream, const std::map<std::string, std::map<std::string, std::string>>& dist) {
	std::vector<std::string> outerkeys = keys(dist), innerkeys = keys(dist.begin()->second);
	int okmw = 0;
	for (auto s : outerkeys) okmw = std::max(okmw, (int)s.length());
	std::map<std::string, int> ikmw;
	for (auto s : outerkeys) {
		for (auto is : innerkeys) {
			ikmw[is] = std::max(ikmw[is], (int)dist.find(s)->second.find(is)->second.length());
		}
	}

	stream << std::string(okmw+1, ' ');
	for (auto is : innerkeys) {
		stream << " " << std::setw(ikmw[is]) << is;
	}
	stream << std::endl;
	for (auto os : outerkeys) {
		stream << std::setw(okmw) << os << ":";
		for (auto is : innerkeys) {
			stream << " " << std::setw(ikmw[is]) << std::setprecision(2) << dist.find(os)->second.find(is)->second;
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

HMM::HMM(const Distribution& pprobs, const std::map<std::string, Distribution>& oprobs, const std::map<std::string, Distribution>& tprobs) : pprobs_(pprobs), oprobs_(oprobs), tprobs_(tprobs), states_(keys(tprobs)) { }

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
	return d;
}

// TODO: Filtering: Given a list of T observations, return the
// posterior probability distribution over the most recent state
// (Given the observations, what is the probability the
// most recent state has each of the possible values)
// (In this case, sunny, rainy, or foggy)
HMM::Distribution HMM::filter(Observations::iterator begin, Observations::iterator end) {
    std::vector<std::string> first;
    for (int i = begin; i < end; i++){
        first.push_back(pprobs[i] + oprobs[i]states[0])
    }
    StringStringFloatMap.push_back(first);
    for (int j = begin; j < states.size(); j++)){
        std::vector<std::string> next;
        for (int i = begin; i < end; i++){
            for (const auto item : logProb){
                logProb = next[j-1][i] + tprobs[i][j];
            }
            next.push_back(logProb + oprobs[i]states[j])
        }
        StringStringFloatMap.push_back(next);
    }
    return StringStringFloatMap;
}


// TODO: Viterbi: Given a list of T observations, return the most
// likely sequence of states (e.g. { "sunny", "rainy", "foggy" ... }
std::vector<std::string> HMM::viterbi(Observations::iterator begin, Observations::iterator end) {
    HMM::Distribution d;
    for (int i = begin+1; i < end; i++){
        std::vector<std::string> temp;
        for (int next = begin; next < end; next++){
            std::vector<std::string> after;
            for (int prev = begin; prev < end; prev++){
                std::vector<std::string> before = StringStringFloatMap[prev];
                before.uniformDistribution += oprobs[next][StringStringFloatMap] + tprobs[prev][next];
                if(before.uniformDistribution > after.uniformDistribution){
                    after.uniformDistribution = before.uniformDistribution;
                }
            }
            temp[next] = after;
        }
        StringStringFloatMap = temp;
        d[i] = StringStringFloatMap;
    }
    for (int i = begin; i < end; i++){
		viterbi.push_back(d[i]);
	}
	return viterbi;
}
