#include <map>
#include "graph.h"

struct ConsensusGroup {
	std::pair<std::string, std::string> nodePair;
	std::vector<Path> paths;

	double  validPathNumber;
	Path consensusSequence;
};

class ConsensusGenerator {
private:
	const int NARROW_RANGE = 10000;
	const int WIDE_RANGE   = 100000;
	const int WINDOW_SIZE  = 1000;

	void createSingleGroup(std::pair<std::string, std::string> pair, std::vector<Path> paths, std::map<long, std::pair<int, double>> info);
	void createMultipleGroups(std::pair<std::string, std::string> pair, std::vector<Path> paths, std::map<long, std::pair<int, double>> info);

public:
	std::map<std::pair<std::string, std::string>, std::vector<ConsensusGroup>> consensusGroups;

	void generateConsensus(std::unordered_set<Path, PathHasher, PathComparator>& paths);
};