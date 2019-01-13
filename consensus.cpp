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
		return p1.averageSeqId < p2.averageSeqId;
	});

	consensus.consensusSequence = *consensusSequence;
	consensus.validPathNumber   =  consensusSequence->averageSeqId;
	consensus.validPathMedian   =  consensusSequence->medianSeqId;

	auto highestFrequency = std::max_element(info.begin(), info.end(), [](const std::pair<long, std::pair<int, double>>& p1, const std::pair<long, std::pair<int, double>>& p2) {
		return p1.second.second < p2.second.second; 
	});
	double frequencyThreshold = highestFrequency->second.second / 2;

	std::copy_if(paths.begin(), paths.end(), std::back_inserter(consensus.paths), [frequencyThreshold, info](const Path& p) {
		return info.at(p.length).second >= frequencyThreshold;
	});
	this->consensusGroups[pair].push_back(consensus);
}

void ConsensusGenerator::createMultipleGroups(std::pair<std::string, std::string> pair, std::vector<Path> paths, std::map<long, std::pair<int, double>> info) {
	auto minLengthPath = std::min_element(paths.begin(), paths.end(), [](const Path& p1, const Path&p2) {
		return p1.length < p2.length;
	});

	long startingLength  = minLengthPath->length;
	     startingLength /= 1000;
		 startingLength  = floor(startingLength + 0.5) * 1000;

	std::vector<ConsensusWindow> windows;
	int numberOfWindows = WIDE_RANGE / WINDOW_SIZE;

	for (int base = 0; base < numberOfWindows; base++) {
		ConsensusWindow window;
		window.baseLength = startingLength + base * WINDOW_SIZE;
		windows.push_back(std::move(window));
	}

	for (auto path : paths) {
		for (int i = 0; i < windows.size(); i++) {
			if (path.length >= windows[i].baseLength && path.length < windows[i].baseLength + WINDOW_SIZE) {
				windows[i].paths.push_back(path);
				break;
			}
		}
	}

	std::vector<ConsensusWindow> filteredWindows;
	std::copy_if(windows.begin(), windows.end(), std::back_inserter(filteredWindows), [](const ConsensusWindow& w) {
		return !w.paths.empty();
	});

	if (filteredWindows.size() < 3) {
		createSingleGroup(pair, paths, info);
	}
	else {
		double maxFreq = std::numeric_limits<double>().min();
		for (auto& window : filteredWindows) {
			for (auto path : window.paths) {
				window.frequenciesSum += info[path.length].second;
			}
			if (window.frequenciesSum > maxFreq) {
				maxFreq = window.frequenciesSum;
			}
		}

		for (auto& window : filteredWindows) {
			window.type = window.frequenciesSum < 0.2 * maxFreq ? VALLEY : PEAK;
		}

		std::vector<long> lengthSplits;
		lengthSplits.push_back(startingLength);
		lengthSplits.push_back(startingLength + WIDE_RANGE);

		for (int index = 1; index < filteredWindows.size() - 1; index++) {
			if (filteredWindows[index].type == VALLEY && filteredWindows[index - 1].type == PEAK && filteredWindows[index + 1].type == PEAK) {
				auto minLengthFreqPath = std::min_element(filteredWindows[index].paths.begin(), filteredWindows[index].paths.end(), [info](const Path& p1, const Path&p2) {
					return info.at(p1.length).second < info.at(p2.length).second;
				});
				lengthSplits.push_back(minLengthFreqPath->length);
			}
		}

		if (lengthSplits.size() > 2) {
			std::sort(lengthSplits.begin(), lengthSplits.end());

			std::vector<std::vector<Path>> splitted(lengthSplits.size() - 1);
			for (auto path : paths) {
				for (int i = 1; i < lengthSplits.size(); i++) {
					if (path.length >= lengthSplits[i - 1] && path.length < lengthSplits[i]) {
						splitted[i - 1].push_back(path);
						break;
					}
				}
			}
			for (auto paths : splitted) {
				createSingleGroup(pair, paths, info);
			}
		}
		else {
			createSingleGroup(pair, paths, info);
		}
	}
}

void ConsensusGenerator::generateConsensus(std::unordered_set<Path, PathHasher, PathComparator>& paths) {
	std::cout << std::endl;
	std::map<std::pair<std::string, std::string>, std::vector<Path>> anchorNodePaths;

	for (auto path : paths) {
		anchorNodePaths[std::make_pair(path.start, path.extensions.back()->nextId)].push_back(path);
	}

	for (auto const& anchorNodePair : anchorNodePaths) {
		long maxLength = std::numeric_limits<long>().min();
		long minLength = std::numeric_limits<long>().max();
		std::map<long, std::pair<int, double>> pathLengthFrequencies;

		for (auto path : anchorNodePair.second) {
			if (path.length > maxLength) {
				maxLength = path.length;
			}
			if (path.length < minLength) {
				minLength = path.length;
			}
			pathLengthFrequencies[path.length].first++;
		}
		for (auto entry : pathLengthFrequencies) {
			pathLengthFrequencies[entry.first].second = (static_cast<double>(entry.second.first) / anchorNodePair.second.size());

			std::cout << anchorNodePair.first.first << " to " << anchorNodePair.first.second << ": (len) " << entry.first  << " -> (freq) " << pathLengthFrequencies[entry.first].second << std::endl;
		}

		long distributionRange = maxLength - minLength;
		if (distributionRange < NARROW_RANGE) {
			createSingleGroup(anchorNodePair.first, anchorNodePair.second, pathLengthFrequencies);
		}
		else if (distributionRange > WIDE_RANGE) {
			//discard completely
		}
		else {
			createMultipleGroups(anchorNodePair.first, anchorNodePair.second, pathLengthFrequencies);
		}

 		std::cout << std::endl;
	}
	std::cout << std::endl;
}