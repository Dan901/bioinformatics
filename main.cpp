#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <math.h>
#include <map>
#include "consensus.h"
#include "connection.h"
#include "node.h"
#include "trail.h"

std::string DNA_NAME = ">EColi ";
std::string FOLDER = "data/EColi/";
std::string CONTIGS_FILE = FOLDER + "ecoli_test_contigs.fasta";
std::string READS_FILE = FOLDER + "ecoli_test_reads.fasta";

std::string READ_CONTIG_OVERLAPS_FILE = FOLDER + "overlaps_reads_contigs.paf";
std::string READ_OVERLAPS_FILE = FOLDER + "overlaps_reads.paf";
std::string OUTPUT_GENOME = FOLDER + "output24";

int OVERLAP_THRESHOLD = 1500;
double SEQUENCE_IDENTITY_THRESHOLD = 0.2;

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
		if (pafLine.queryId == pafLine.targetId || pafLine.isFullyContained() || pafLine.overlapScore < OVERLAP_THRESHOLD || pafLine.sequenceId < SEQUENCE_IDENTITY_THRESHOLD) {
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
bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

//contig
std::map<std::string, std::string> readFasta(std::string file) {
	std::ifstream input(file);
	std::string line;
	std::map<std::string, std::string> output;
	bool isFastQ = false;
	if(hasEnding(file, "fastq")){
		isFastQ = true;
	}

	bool data = false;
	std::string name;
	std::string bases;
	int skip = 0;
	while (std::getline(input, line)) {
		if (skip > 0) {
			skip--;
			continue;
		}
		if (data) {
			bases = line;
			output[name] = bases;
			if (isFastQ) {
				skip = 2;
			}
		} else {
			name = split(line.substr(1, line.size()), ' ').at(0);
		}
		data = !data;
	}
	return output;
}

std::string invertDNA(std::string toInvert) {
	std::string inverted;
	for (auto dnaChar : toInvert) {
		switch (dnaChar) {
		case 'A': inverted = "T" + inverted;
			break;
		case 'T': inverted = "A" + inverted;
			break;
		case 'C': inverted = "G" + inverted;
			break;
		case 'G': inverted = "C" + inverted;
			break;
		}
	}
	return inverted;
}

std::vector<Trail> findTrails(std::string currentContig, std::map<std::string, Node*>& nodes, std::vector<Trail>& currentTrails, Trail currentTrail){
	// std::cout << "Visited node "<< currentContig << " atfer findTrails called should be true" << nodes[currentContig]->visited  << std::endl;
	// std::cout << "Number of vertices for this node "<< nodes[currentContig]->vertices.size() << std::endl;
	for(auto nextElement : nodes[currentContig]->vertices){
		std::string nextContig = nextElement.first;
		// std::cout<< "Next contig " << nextContig << std::endl;
		if(nodes[nextContig]->visited){
			continue;
		}
		// std::cout<< "Still Next contig " << nextContig << std::endl;
		currentTrail.goodness += nextElement.second;
		currentTrail.trail.push_back(std::make_pair(currentContig, nextContig));
		nodes[nextContig]->visited = true;
		currentTrails = findTrails(nextContig, nodes, currentTrails, currentTrail);
		nodes[nextContig]->visited = false;
		currentTrail.goodness -= nextElement.second;
		currentTrail.trail.pop_back();
	}
	currentTrails.push_back(currentTrail);
	return currentTrails;
}

bool trailSorterByGoodness(Trail train1, Trail trail2){
	return (train1.goodness < trail2.goodness);
}

bool trailSorterByLengthAndGoodness(Trail train1, Trail trail2){
	if(train1.trail.size() == trail2.trail.size()){
		return train1.goodness < trail2.goodness;
	}
	return (train1.trail.size() < trail2.trail.size());
}
int main(int argc, char** argv) {
	if(argc == 4) {
		FOLDER		 = argv[1];
		CONTIGS_FILE = FOLDER + argv[2];
		READS_FILE   = FOLDER + argv[3];

		READ_CONTIG_OVERLAPS_FILE = FOLDER + "overlaps_reads_contigs.paf";
		READ_OVERLAPS_FILE		  = FOLDER + "overlaps_reads.paf";
		OUTPUT_GENOME		      = FOLDER + "output1";
	}
	else {
		std::cout << "Additional arguments not detected, using default ones..." << std::endl;
	}

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

	//as there can be more conections stemming from the same contig
	std::map<std::pair<std::string, std::string>, Connection> contigConnections;
	//Resolved contig and its connection to the nest contig
	// std::map<std::string, Connection> pathConnection;
	std::map<std::string, Node*> nodes;

	for(auto contig : graph.contigIds){
		Node * node = new Node();
		node->contigId = contig;
		nodes[contig] = node;
	}

	for (auto group : gen.consensusGroups) {
		std::cout << "First group of contigs" << group.first.first << group.first.second << std::endl;
		for(auto pairGroup : group.second){
			if(contigConnections.find(group.first) != contigConnections.end() && pairGroup.validPathNumber < contigConnections[group.first].validPathNumber){
				continue;
			}

			Connection connection;
			connection.validPathNumber = pairGroup.validPathNumber;
			connection.path = pairGroup.consensusSequence;
			contigConnections[group.first] = connection;

			//Adding to cotg first vertex to second ctg 
			double contigConnectionValue = nodes[group.first.first]->vertices[group.first.second];
			if(contigConnectionValue < pairGroup.validPathNumber){
				nodes[group.first.first]->vertices[group.first.second] = pairGroup.validPathNumber; 
			}
		}
	}

	// you have to sort this!!!!
	std::vector<Trail> trails;
	for(auto elements: nodes){
		std::string currentContigsId = elements.first;
		nodes[currentContigsId]->visited = true;	
		std::cout<< currentContigsId << std::endl;
		trails = findTrails(currentContigsId, nodes, trails, Trail());
		nodes[currentContigsId]->visited = false;	

		// std::cout << "For node " << elements.first << std::endl;
		// for(auto vertex : elements.second.vertices){
		// 	std::cout << "connection to " << vertex.first << " - value " << vertex.second << std::endl;
		// }
	}

	for(auto trail : trails){
		std::cout << "Benefit : " << trail.goodness << std::endl;
		for(auto element : trail.trail){
			std::cout << element.first << "->" << element.second << std::endl;
		}
	}

	std::cout << "Number of trails found :"<< trails.size()-1 << std::endl;

	// SORT BY GOODNES 
	// std::sort(trails.begin(), trails.end(), trailSorterByGoodnes);
	// std::cout << "Best by goodnes" << std::endl;
	// std::vector<Trail> bestTrailsByGoodnes;
	// for(int i = 0; i < 3; i ++){
	// 	Trail curr = trails.at(trails.size() - i - 1);
	// 	bestTrailsByGoodnes.push_back(curr);
	// 	std::cout << "Benefit : " << curr.goodnes << std::endl;
	// 	for(auto element : curr.trail){
	// 		std::cout << element.first << "->" << element.second << std::endl;
	// 	}
	// }

	// SORT BY LENGTH and then goodnes
	std::sort(trails.begin(), trails.end(), trailSorterByLengthAndGoodness);
	
	std::cout << "Best by length" << std::endl;
	std::vector<Trail> bestTrailsByLength;
	for(int i = trails.size()-1; i  >= trails.size() - 3 && i >= 0; i --){
		Trail curr = trails.at(i);
		bestTrailsByLength.push_back(curr);
		std::cout << "Benefit : " << curr.goodness << std::endl;
		for(auto element : curr.trail){
			std::cout << element.first << "->" << element.second << std::endl;
		}
	}

	std::cout << "Reading contigs ..." << std::endl;
	std::map<std::string, std::string> contigs = readFasta(CONTIGS_FILE);
	std::cout << "Done reading contigs." << std::endl;

	std::cout << "Reading reads ..." << std::endl;
	std::map<std::string, std::string> reads = readFasta(READS_FILE);
	std::cout << "Done reading reads." << std::endl;

	for(auto trail : bestTrailsByLength){
		std::cout << "Currently resolving path: "<< trail.getName() << std::endl;
		std::ofstream output;
		output.open(OUTPUT_GENOME +"_"+ trail.getName() + ".fasta");
		output << DNA_NAME + " " + trail.getName() + "\n";
		bool isFirst = true;
		int lastExtensionStart = 0;
		int lastExtensionEnd = -1;
		int lastExtensionLength = -1;
		int sameStrand = true;
		int inverted = false;
		std::string lastReadId;
		std::string lastContig;

		for(auto contigPair : trail.trail){
			Connection connection = contigConnections[contigPair];
			std::cout << "Currently resolving contig connection: " << contigPair.first << "-" << contigPair.second << std::endl;
			for (auto extension : connection.path.extensions) {
				if (isFirst) {
					if (!inverted) {
						output << (contigs.find(contigPair.first)->second).substr(lastExtensionStart, extension->lastStart);
					} else {
						output << invertDNA((contigs.find(contigPair.first)->second).substr(lastExtensionEnd, extension->lastEnd));
					}
					isFirst = false;
				} else {
					if (!inverted) {
						output << (reads.find(lastReadId)->second).substr(lastExtensionStart, extension->lastStart);
					} else {
						output << invertDNA((reads.find(lastReadId)->second).substr(lastExtensionEnd, extension->lastEnd));
					}
				}
				lastExtensionStart = extension->nextStart;
				lastExtensionEnd = extension->nextEnd;
				lastReadId = extension->nextId;
				lastExtensionLength = extension->nextLen;
				sameStrand = extension->sameStrand;
				if (!sameStrand) {
					inverted = !inverted;
				}
			}
			isFirst = true;
			output.flush();
			lastContig = contigPair.second;
		}
	
		if (!inverted) {
			output << (contigs.find(lastContig)->second).substr(lastExtensionStart, lastExtensionLength);
		} else {
			output << invertDNA((contigs.find(lastContig)->second).substr(0, lastExtensionEnd));
		}
		output.flush();
		output.close();
	}

	std::cout << "End" << std::endl;
	return 0;
}