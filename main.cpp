#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <math.h>

#include "anchor.h"
#include "consensus.h"

std::string READ_CONTIG_OVERLAPS_FILE = "data/CJejuni/overlaps_reads_contigs.paf";
std::string READ_OVERLAPS_FILE = "data/CJejuni/overlaps_reads.paf";
double CONFLICT_INDEX = 0.75; 
int OVERLAP_THRESHOLD = 1000;

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
std::vector<PafLine> readAndFilterLines(std::string file, std::unordered_set<std::string> * contigIds) {
	std::ifstream input(file);
	std::string line;
	std::vector<PafLine> filteredLines;

	while (std::getline(input, line)) {
		PafLine pafLine(split(line, '\t'));
		if (pafLine.queryId == pafLine.targetId || pafLine.isFullyContained() || pafLine.overlapScore < OVERLAP_THRESHOLD) {
			continue;
		}

		if (contigIds) {
			contigIds->insert(pafLine.queryId);
		}

		filteredLines.push_back(pafLine);
	}

	return filteredLines;
}

Graph constructGraph(std::vector<ExtensionSelector*> extensionSelectors) {
	std::unordered_set<std::string> contigIds;
	std::vector<PafLine> readContigOverlaps = readAndFilterLines(READ_CONTIG_OVERLAPS_FILE, &contigIds);
	std::vector<PafLine> readOverlaps = readAndFilterLines(READ_OVERLAPS_FILE, nullptr);

	Graph graph(contigIds, extensionSelectors);

	for (auto line : readContigOverlaps) {
		graph.insertExtensions(line);
	}

	for (auto line : readOverlaps) {
		graph.insertExtensions(line);
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
		std::cout << path.start << " -> " << path.extensions.back()->nextId << "\t" << path.extensions.size() << "\t" << path.length << std::endl;
	}

	ConsensusGenerator gen;
	gen.generateConsensus(uniquePaths);

	std::map<std::string, Anchor> anchors;

	for (auto group : gen.consensusGroups){
		ConsensusGroup consensus = group.second.front();
		Anchor anchor;
		anchor.contig = group.first.first;
		if(anchors.count(group.first.first)> 0){
			anchor = anchors.find(group.first.first)->second;
		}

		if(consensus.validPathNumber > anchor.firstNP){
			anchor.secondNP = anchor.firstNP;
			anchor.firstNP = consensus.validPathNumber;
		}else if(consensus.validPathNumber > anchor.firstNP){
			anchor.secondNP = consensus.validPathNumber;
		}

		anchors.insert(std::make_pair(group.first.first, anchor));
	}

	for(auto anchor : anchors){
		anchor.second.calculateConflictIndex();
		std::cout << "Contig: "<< anchor.second.contig <<" Best NP: " << anchor.second.firstNP  << " Second best NP: " << anchor.second.secondNP << std::endl;
		std::cout << "CI: " << anchor.second.conflictIndex << std::endl;
	}

	std::cout << "End" << std::endl;
	return 0;
}