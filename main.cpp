#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "graph.h"

std::vector<std::string> split(std::string line, char delim) {
	std::vector<std::string> elements;
	std::stringstream ss(line);
	std::string item;

	while (std::getline(ss, item, delim)) {
		elements.push_back(item);
	}

	return elements;
}

std::vector<Overlap> readAndFilterOverlaps(std::string file, double threshold) {
	std::ifstream input(file);
	std::string line;
	std::vector<Overlap> readContigOverlaps;

	while (std::getline(input, line)) {
		readContigOverlaps.emplace_back(split(line, '\t'));
	}
	input.close();

	std::vector<Overlap> filteredReadContigOverlaps;
	std::copy_if(readContigOverlaps.begin(), readContigOverlaps.end(), std::back_inserter(filteredReadContigOverlaps),
		[threshold](Overlap o) {
		if (o.isFullyContained()) {
			return false;
		}
		return o.extensionScore1 > threshold || o.extensionScore2 > threshold;
	});

	return filteredReadContigOverlaps;
}

int main() {
	std::string readContigOverlapsFile = "C:\\FER - local\\Bioinformatika\\data\\BGrahamii - real\\overlaps_reads_contigs.paf";
	std::string readOverlapsFile = "C:\\FER - local\\Bioinformatika\\data\\BGrahamii - real\\overlaps_reads.paf";

	std::vector<Overlap> readContigOverlaps = readAndFilterOverlaps(readContigOverlapsFile, 0);
	std::vector<Overlap> readOverlaps = readAndFilterOverlaps(readOverlapsFile, 0);

	Graph graph;
	for (auto overlap : readContigOverlaps) {
		if (overlap.extensionScore1 > overlap.extensionScore2) {
			graph.contigs[overlap.queryId].rightExtensions.push_back(overlap);
			graph.reads[overlap.targetId].leftExtensions.push_back(overlap);
		} else {
			graph.contigs[overlap.queryId].leftExtensions.push_back(overlap);
			graph.reads[overlap.targetId].rightExtensions.push_back(overlap);
		}
	}

	for (auto overlap : readOverlaps) {
		Node queryNode = graph.reads[overlap.queryId];
		Node targetNode = graph.reads[overlap.targetId];

		if (overlap.extensionScore1 > overlap.extensionScore2) {
			queryNode.rightExtensions.push_back(overlap);
			targetNode.leftExtensions.push_back(overlap);
		} else {
			queryNode.leftExtensions.push_back(overlap);
			targetNode.rightExtensions.push_back(overlap);
		}
	}

	std::cout << "Pog";

	return 0;
}