#include "consensus.h"
#include <set>
#include <iostream>
#include <algorithm>
#include <limits>
#include <cmath>

void ConsensusGenerator::createSingleGroup(std::pair<std::string, std::string> pair, std::vector<Path> paths, std::map<long, std::pair<int, double>> info) {
	ConsensusGroup consensus;
	consensus.nodePair = pair;

	//finds path with maximal average sequence identity
	auto consensusSequence = std::max_element(paths.begin(), paths.end(), [](const Path& p1, const Path&p2) {
		return p1.averageSeqId < p2.averageSeqId;
	});

	consensus.consensusSequence = *consensusSequence;
	consensus.validPathNumber   =  consensusSequence->averageSeqId;
	consensus.validPathMedian   =  consensusSequence->medianSeqId;

	//finds path with highest length frequency appearance
	auto highestFrequency = std::max_element(info.begin(), info.end(), [](const std::pair<long, std::pair<int, double>>& p1, const std::pair<long, std::pair<int, double>>& p2) {
		return p1.second.second < p2.second.second; 
	});
	double frequencyThreshold = highestFrequency->second.second / 2;

	//filters out paths whose length frequency isn't at least 50% of the highest length frequency
	std::copy_if(paths.begin(), paths.end(), std::back_inserter(consensus.paths), [frequencyThreshold, info](const Path& p) {
		return info.at(p.length).second >= frequencyThreshold;
	});
	this->consensusGroups[pair].push_back(consensus);
}

void ConsensusGenerator::createMultipleGroups(std::pair<std::string, std::string> pair, std::vector<Path> paths, std::map<long, std::pair<int, double>> info) {
	//find path of minimal length
	auto minLengthPath = std::min_element(paths.begin(), paths.end(), [](const Path& p1, const Path&p2) {
		return p1.length < p2.length;
	});

	//use minimal length as base of window generation
	long startingLength  = minLengthPath->length;
	     startingLength /= 1000;
		 startingLength  = floor(startingLength + 0.5) * 1000;

	std::vector<ConsensusWindow> windows;
	int numberOfWindows = WIDE_RANGE / WINDOW_SIZE;

	//create windows that cover the wide distribution range starting from found minimal length
	for (int base = 0; base < numberOfWindows; base++) {
		ConsensusWindow window;
		window.baseLength = startingLength + base * WINDOW_SIZE;
		windows.push_back(std::move(window));
	}

	//group found paths into their respective windows
	for (auto path : paths) {
		for (int i = 0; i < windows.size(); i++) {
			if (path.length >= windows[i].baseLength && path.length < windows[i].baseLength + WINDOW_SIZE) {
				windows[i].paths.push_back(path);
				break;
			}
		}
	}

	//filter out windows which do not have any paths in them (zero-sized ones)
	std::vector<ConsensusWindow> filteredWindows;
	std::copy_if(windows.begin(), windows.end(), std::back_inserter(filteredWindows), [](const ConsensusWindow& w) {
		return !w.paths.empty();
	});

	if (filteredWindows.size() < 3) {
		//if there are less than 3 windows, group all paths into same group
		createSingleGroup(pair, paths, info);
	}
	else {
		//compute maximum sum of path frequencies across generated windows
		double maxFreq = std::numeric_limits<double>().min();
		for (auto& window : filteredWindows) {
			for (auto path : window.paths) {
				window.frequenciesSum += info[path.length].second;
			}
			if (window.frequenciesSum > maxFreq) {
				maxFreq = window.frequenciesSum;
			}
		}

		//label windows whose sum of path frequencies is less than 20% of the highest one as valleys, otherwise as peaks
		for (auto& window : filteredWindows) {
			window.type = window.frequenciesSum < 0.2 * maxFreq ? VALLEY : PEAK;
		}

		std::vector<long> lengthSplits;
		lengthSplits.push_back(startingLength);
		lengthSplits.push_back(startingLength + WIDE_RANGE);

		//find all PEAK-VALLEY-PEAK window sequences and, if such exist, save the path length of lowest frequency in the sequence valley window
		for (int index = 1; index < filteredWindows.size() - 1; index++) {
			if (filteredWindows[index].type == VALLEY && filteredWindows[index - 1].type == PEAK && filteredWindows[index + 1].type == PEAK) {
				auto minLengthFreqPath = std::min_element(filteredWindows[index].paths.begin(), filteredWindows[index].paths.end(), [info](const Path& p1, const Path&p2) {
					return info.at(p1.length).second < info.at(p2.length).second;
				});
				lengthSplits.push_back(minLengthFreqPath->length);
			}
		}

		//if 'N' sequences are found, split paths into 'N + 1' groups and out form consensus groups out of each split
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
		//otherwise slam all of them into same group
		else {
			createSingleGroup(pair, paths, info);
		}
	}
}

void ConsensusGenerator::generateConsensus(std::unordered_set<Path, PathHasher, PathComparator>& paths) {
	std::cout << std::endl;
	std::map<std::pair<std::string, std::string>, std::vector<Path>> anchorNodePaths;

	//group paths which connect same contigs
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
			std::vector<Path> novi(anchorNodePair.second.size());
			std::copy(anchorNodePair.second.begin(), anchorNodePair.second.end(), novi.begin());
			std::vector<Path> novi2(novi.size());
			std::sort(novi.begin(), novi.end(), less_than_key());
			std::copy(novi.begin(), novi.end(), novi2.begin());

			while (novi.back().length - novi.front().length > WIDE_RANGE) {
				novi.pop_back();
			}
			while (novi2.back().length - novi2.front().length > WIDE_RANGE) {
				novi2.erase(novi2.begin());
			}

			bool takeFirst = novi.size() > novi2.size();
			if (novi.size() == novi2.size()) {
				takeFirst = novi.front().length < novi2.front().length;
			}

			createMultipleGroups(anchorNodePair.first, takeFirst ? novi : novi2, pathLengthFrequencies);
		}
		else {
			createMultipleGroups(anchorNodePair.first, anchorNodePair.second, pathLengthFrequencies);
		}

 		std::cout << std::endl;
	}
	std::cout << std::endl;
}