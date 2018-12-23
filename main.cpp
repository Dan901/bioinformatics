#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
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
std::vector<Overlap> readAndFilterOverlaps(std::string file, double threshold) {
	std::ifstream input(file);
	std::string line;
	std::vector<Overlap> filteredOverlaps;

	while (std::getline(input, line)) {
		Overlap overlap = Overlap(split(line, '\t'));
		if (overlap.isFullyContained() || overlap.getExtensionScore() < threshold) {
			continue;
		}

		filteredOverlaps.push_back(std::move(overlap)); // avoid copy
	}
	input.close();

	return filteredOverlaps;
}

int main() {
	std::vector<Overlap> readContigOverlaps = readAndFilterOverlaps(READ_CONTIG_OVERLAPS_FILE, 0);
	std::vector<Overlap> readOverlaps = readAndFilterOverlaps(READ_OVERLAPS_FILE, 0);

	Graph graph;
	for (auto overlap : readContigOverlaps) {
		graph.insertReadContigOverlap(overlap);
	}

	for (auto overlap : readOverlaps) {
		graph.insertOverlap(overlap);
	}

	for (auto contig : graph.contigIds) {
		std::cout << contig << std::endl;
	}

	return 0;
}