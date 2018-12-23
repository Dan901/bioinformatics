#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

#include "graph.h"

std::string READ_CONTIG_OVERLAPS_FILE = "data/BGrahamii/overlaps_reads_contigs.paf";
std::string READ_OVERLAPS_FILE = "data/BGrahamii/overlaps_reads.paf";

std::vector<std::string> split(std::string line, char delim) {
	std::vector<std::string> elements;
	std::stringstream ss(line);
	std::string item;

	while (std::getline(ss, item, delim)) {
		elements.push_back(item);
	}

	return elements;
}

// discard overlaps that are fully contained or have extension score below thershold
// and fill contigIds set
std::vector<Overlap> readAndfilterReadContigOverlaps(std::string file, double threshold, std::unordered_set<std::string>& contigIds) {
	std::ifstream input(file);
	std::string line;
	std::vector<Overlap> filteredOverlaps;

	while (std::getline(input, line)) {
		PafLine pafLine(split(line, '\t'));
		if (pafLine.isFullyContained() || pafLine.getExtensionScore() < threshold) {
			continue;
		}

		contigIds.insert(pafLine.queryId);
		filteredOverlaps.emplace_back(pafLine);
	}

	return filteredOverlaps;
}

// discard overlaps that are fully contained or have extension score below thershold
std::vector<Overlap> readAndfilterReadOverlaps(std::string file, double threshold) {
	std::ifstream input(file);
	std::string line;
	std::vector<Overlap> filteredOverlaps;

	while (std::getline(input, line)) {
		PafLine pafLine(split(line, '\t'));
		if (pafLine.isFullyContained() || pafLine.getExtensionScore() < threshold) {
			continue;
		}

		filteredOverlaps.emplace_back(pafLine);
	}

	return filteredOverlaps;
}

Graph constructGraph() {
	std::unordered_set<std::string> contigIds;
	std::vector<Overlap> readContigOverlaps = readAndfilterReadContigOverlaps(READ_CONTIG_OVERLAPS_FILE, 0, contigIds);
	std::vector<Overlap> readOverlaps = readAndfilterReadOverlaps(READ_OVERLAPS_FILE, 0);

	Graph graph(contigIds);

	for (auto overlap : readContigOverlaps) {
		graph.insertOverlap(overlap);
	}

	for (auto overlap : readOverlaps) {
		graph.insertOverlap(overlap);
	}

	return graph;
}

int main() {
	Graph graph = constructGraph();

	std::cout << "End" << std::endl;
	return 0;
}