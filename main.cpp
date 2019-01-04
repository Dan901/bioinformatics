#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

#include "consensus.h"

std::string READ_CONTIG_OVERLAPS_FILE = "data/EColi/overlaps_reads_contigs.paf";
std::string READ_OVERLAPS_FILE = "data/EColi/overlaps_reads.paf";
int EXTENSION_THRESHOLD = 0;

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
// and fill contigIds set if it is not null
std::vector<Overlap> readAndfilterOverlaps(std::string file, double threshold, std::unordered_set<std::string> * contigIds) {
	std::ifstream input(file);
	std::string line;
	std::vector<Overlap> filteredOverlaps;

	while (std::getline(input, line)) {
		PafLine pafLine(split(line, '\t'));
		if (pafLine.queryId == pafLine.targetId || pafLine.isFullyContained() || pafLine.getExtensionScore() < threshold) {
			continue;
		}

		if (contigIds != nullptr) {
			contigIds->insert(pafLine.queryId);
		}

		filteredOverlaps.emplace_back(pafLine);
	}

	return filteredOverlaps;
}

Graph constructGraph(std::vector<ExtensionSelector*> extensionSelectors) {
	std::unordered_set<std::string> contigIds;
	std::vector<Overlap> readContigOverlaps = readAndfilterOverlaps(READ_CONTIG_OVERLAPS_FILE, EXTENSION_THRESHOLD, &contigIds);
	std::vector<Overlap> readOverlaps = readAndfilterOverlaps(READ_OVERLAPS_FILE, EXTENSION_THRESHOLD, nullptr);

	Graph graph(contigIds, extensionSelectors);

	for (auto overlap : readContigOverlaps) {
		graph.insertOverlap(overlap);
	}

	for (auto overlap : readOverlaps) {
		graph.insertOverlap(overlap);
	}

	return graph;
}

int main() {
	BestExtensionSelector bestOS = BestExtensionSelector(compareByOverlapScore);
	BestExtensionSelector bestES = BestExtensionSelector(compareByExtensionScore);

	std::vector<ExtensionSelector*> extensionSelectors;
	extensionSelectors.push_back(&bestOS);
	extensionSelectors.push_back(&bestES);

	Graph graph = constructGraph(extensionSelectors);
	std::unordered_set<Path, PathHasher, PathComparator> uniquePaths;
	int pathCount = 0;

	for (auto contig : graph.contigIds) {
		std::vector<Path> paths = graph.constructPaths(contig);

		std::cout << "Paths from " << contig << ": " << paths.size() << std::endl;

		for (auto path : paths) {
			pathCount++;
			uniquePaths.insert(path);
		}
	}

	std::cout << std::endl << "Total paths:" << pathCount << std::endl;
	std::cout << "Unique paths:" << uniquePaths.size() << std::endl << std::endl;
	for (auto path : uniquePaths) {
		std::cout << path.start << " -> " << path.overlaps.back()->rightId << "\t" << path.overlaps.size() << "\t" << path.length << std::endl;
	}

	ConsensusGenerator gen;
	gen.generateConsensus(uniquePaths);



	std::cout << "End" << std::endl;
	return 0;
}