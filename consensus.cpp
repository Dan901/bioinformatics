#include "consensus.h"
#include <set>
#include <iostream>
#include <algorithm>
#include <limits>
#include <cmath>

void ConsensusGenerator::createSingleGroup(std::pair<std::string, std::string> pair, std::vector<Path> paths, std::map<long, std::pair<int, double>> info) {
	ConsensusGroup consensus;

	consensus.nodePair = pair;

	auto consensusSequence = std::max_element(paths.begin(), paths.end(), [](const Path& p1, const Path&p2) {
		return p1.average_seq_id < p2.average_seq_id;
	});

	consensus.consensusSequence = *consensusSequence;
	consensus.validPathNumber   =  consensusSequence->average_seq_id;

	auto highest_frequency = std::max_element(info.begin(), info.end(), [](const std::pair<long, std::pair<int, double>>& p1, const std::pair<long, std::pair<int, double>>& p2) {
		return p1.second.second < p2.second.second; 
	});
	double frequency_threshold = highest_frequency->second.second / 2;

	std::copy_if(paths.begin(), paths.end(), std::back_inserter(consensus.paths), [frequency_threshold, info](const Path& p) {
		return info.at(p.length).second >= frequency_threshold;
	});
	this->consensusGroups[pair].push_back(consensus);
}

void ConsensusGenerator::createMultipleGroups(std::pair<std::string, std::string> pair, std::vector<Path> paths, std::map<long, std::pair<int, double>> info) {

}

void ConsensusGenerator::generateConsensus(std::unordered_set<Path, PathHasher, PathComparator>& paths) {
	std::cout << std::endl;
	std::map<std::pair<std::string, std::string>, std::vector<Path>> anchorNodePaths;

	for (auto path : paths) {
		anchorNodePaths[std::make_pair(path.start, path.extensions.back()->nextId)].push_back(path);
	}

	for (auto const& anchorNodePair : anchorNodePaths) {
		long max_length = std::numeric_limits<long>().min();
		long min_length = std::numeric_limits<long>().max();
		std::map<long, std::pair<int, double>> pathLengthFrequencies;

		for (auto path : anchorNodePair.second) {
			if (path.length > max_length) {
				max_length = path.length;
			}
			if (path.length < min_length) {
				min_length = path.length;
			}
			pathLengthFrequencies[path.length].first++;
		}
		for (auto entry : pathLengthFrequencies) {
			pathLengthFrequencies[entry.first].second = (static_cast<double>(entry.second.first) / anchorNodePair.second.size());

			std::cout << anchorNodePair.first.first << " to " << anchorNodePair.first.second << ": (len) " << entry.first  << " -> (freq) " << pathLengthFrequencies[entry.first].second << std::endl;
		}

		long distributionRange = max_length - min_length;
		if (distributionRange < NARROW_RANGE) {
			createSingleGroup(anchorNodePair.first, anchorNodePair.second, pathLengthFrequencies);
		}
		else if (distributionRange > WIDE_RANGE) {
			//discard completely
		}
		else {
			createSingleGroup(anchorNodePair.first, anchorNodePair.second, pathLengthFrequencies);
		}

 		std::cout << std::endl;
	}
	std::cout << std::endl;
}