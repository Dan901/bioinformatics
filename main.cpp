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

std::string DNA_NAME = ">CJejuni\n";
std::string FOLDER = "data/CJejuni/";
std::string CONTIGS_FILE = FOLDER + "CJejuni - contigs.fasta";
std::string READS_FILE = FOLDER + "CJejuni - reads.fastq";

std::string READ_CONTIG_OVERLAPS_FILE = FOLDER + "overlaps_reads_contigs.paf";
std::string READ_OVERLAPS_FILE = FOLDER + "overlaps_reads.paf";
std::string OUTPUT_GENOME = FOLDER + "output9.fasta";

int OVERLAP_THRESHOLD = 1000;
double SEQUENCE_IDENTITY_THRESHOLD = 0.3;

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

//contig
std::map<std::string, std::string> readFasta(std::string file, bool realFasta) {
	std::ifstream input(file);
	std::string line;
	std::map<std::string, std::string> output;

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
			if (realFasta) {
				skip = 2;
			}
		} else {
			name = line.substr(1, line.size());
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

	//as there can be more conections stemming from the same contig
	std::map<std::string, std::vector<Connection>> connections;
	//Resolved contig and its connection to the nest contig
	std::map<std::string, Connection> pathConnection;

	for (auto group : gen.consensusGroups) {
		std::cout << "First group of contigs" << group.first.first << group.first.second << std::endl;
		Connection connection;
		connection.contigId = group.first.second;
		connection.validPathNumber = group.second.front().validPathNumber;
		connection.path = group.second.front().consensusSequence;
		connections[group.first.first].push_back(connection);
	}

	//choose the best path if more contigs go to the same one
	//TODO what if there is one contige whose two
	for (auto connectionPair : connections) {
		Connection best = connectionPair.second.front();
		for (auto connection : connectionPair.second) {
			std::cout << "Contig " << connectionPair.first << "and contig's " << connection.contigId << "is" << connection.validPathNumber << std::endl;
			if (best.validPathNumber < connection.validPathNumber) {
				best = connection;
			}
		}
		std::cout << "Contig pair" << connectionPair.first << "-" << best.contigId << std::endl;
		pathConnection[connectionPair.first] = best;
	}

	//TODO cycle for broken contig connections

	// FIX wrong 
	std::string firstContig;
	bool first = true;
	for (auto contig : graph.contigIds) {
		for (auto connection : pathConnection) {
			if (connection.second.contigId == contig) {
				first = false;
			}
		}
		if (first) {
			firstContig = contig;
		}
		first = true;
	}

	std::cout << "First contig in the chain: " << firstContig << std::endl;

	std::cout << "Reading contigs ..." << std::endl;
	std::map<std::string, std::string> contigs = readFasta(CONTIGS_FILE, false);
	std::cout << "Done reading contigs." << std::endl;

	std::cout << "Reading reads ..." << std::endl;
	std::map<std::string, std::string> reads = readFasta(READS_FILE, true);
	std::cout << "Done reading reads." << std::endl;


	std::string nextContig = firstContig;
	std::ofstream output;
	output.open(OUTPUT_GENOME);
	output << DNA_NAME;
	bool isFirst = true;
	int lastExtensionStart = 0;
	int lastExtensionEnd = -1;
	int lastExtensionLength = -1;
	int sameStrand = true;
	int inverted = false;
	std::string lastReadId;

	while (pathConnection.find(nextContig) != pathConnection.end()) {
		Connection connection = pathConnection[nextContig];
		std::cout << "Currently resolving contig connection: " << nextContig << "-" << connection.contigId << std::endl;
		for (auto extension : connection.path.extensions) {
			if (isFirst) {
				if (!inverted) {
					output << (contigs.find(nextContig)->second).substr(lastExtensionStart, extension->lastStart);
				} else {
					output << invertDNA((contigs.find(nextContig)->second).substr(lastExtensionStart, extension->lastStart));
				}
				isFirst = false;
			} else {
				if (!inverted) {
					output << (reads.find(lastReadId)->second).substr(lastExtensionStart, extension->lastStart);
				} else {
					output << invertDNA((reads.find(lastReadId)->second).substr(lastExtensionStart, extension->lastStart));
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
		nextContig = connection.contigId;
		output.flush();
	}
	if (!inverted) {
		output << (contigs.find(nextContig)->second).substr(lastExtensionStart, lastExtensionLength);
	} else {
		output << invertDNA((contigs.find(nextContig)->second).substr(lastExtensionStart, lastExtensionLength));
	}

	output.close();

	std::cout << "End" << std::endl;
	return 0;
}