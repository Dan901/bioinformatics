#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "overlap.h"

struct Node {
	std::vector<Overlap> rightExtensions;
	std::vector<Overlap> leftExtensions;
};

class Graph {
public:
	std::unordered_set<std::string> contigIds;

	void insertReadContigOverlap(Overlap& overlap);
	void insertOverlap(Overlap& overlap);

private:
	std::unordered_map<std::string, Node> nodes;
};