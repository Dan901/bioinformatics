#include <map>
#include "graph.h"

enum WindowType {PEAK, VALLEY};

struct ConsensusGroup {
	std::pair<std::string, std::string> nodePair;
	std::vector<Path> paths;

	double  validPathNumber;
	double  validPathMedian;
	Path consensusSequence;
};

struct ConsensusWindow {
	std::vector<Path> paths;
	long baseLength;
	WindowType type;

	double frequenciesSum = 0;
};

class ConsensusGenerator {
private:
	const int NARROW_RANGE = 10000;
	const int WIDE_RANGE   = 100000;
	const int WINDOW_SIZE  = 10000;

	void createSingleGroup(std::pair<std::string, std::string> pair, std::vector<Path> paths, std::map<long, std::pair<int, double>> info);
	void createMultipleGroups(std::pair<std::string, std::string> pair, std::vector<Path> paths, std::map<long, std::pair<int, double>> info);

public:
	std::map<std::pair<std::string, std::string>, std::vector<ConsensusGroup>> consensusGroups;

	void generateConsensus(std::unordered_set<Path, PathHasher, PathComparator>& paths);
};