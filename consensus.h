#include <map>
#include "graph.h"

//types of subgroups
enum WindowType {PEAK, VALLEY};

//groups from which the consensus sequence is extracted.
struct ConsensusGroup {
	std::pair<std::string, std::string> nodePair;
	std::vector<Path> paths;

	double  validPathNumber;
	double  validPathMedian;
	Path consensusSequence;
};

//subgroup windows used to try to divide the larger consensus groups to a few smaller ones.
struct ConsensusWindow {
	std::vector<Path> paths;
	long baseLength;
	WindowType type;

	double frequenciesSum = 0;
};

struct less_than_key {
	inline bool operator() (const Path& p1, const Path& p2) {
		return (p1.length < p2.length);
	}
};

//Class providing the consensus resolution utility. 
class ConsensusGenerator {
private:
	const int NARROW_RANGE = 10000;
	const int WIDE_RANGE   = 100000;
	const int WINDOW_SIZE  = 10000;

	//creates a single consensus group out of given parameters. It takes a pair of contigs, all found paths between them,
	//and statistical information about those paths (frequencies of appeareance of given path lengths). Called if found paths
	//are distributed narrowly (within 10 kilobases)
	void createSingleGroup(std::pair<std::string, std::string> pair, std::vector<Path> paths, std::map<long, std::pair<int, double>> info);

	//tries to split set of given paths into multiple consensus groups. Takes same arguments as its single group counterpart, but it is called
	//if the path distribution is wider than 10 kilobases but not wider than 100 kilobases
	void createMultipleGroups(std::pair<std::string, std::string> pair, std::vector<Path> paths, std::map<long, std::pair<int, double>> info);

public:
	std::map<std::pair<std::string, std::string>, std::vector<ConsensusGroup>> consensusGroups;

	//starts the consensus generation process. After it has finished, consensusGroups member will contain
	//consensus groups for each pair of found contigs
	void generateConsensus(std::unordered_set<Path, PathHasher, PathComparator>& paths);
};