#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "path.h"

struct Node {
	std::vector<Overlap> extensions;
};

class Graph {
public:
	std::unordered_set<std::string> contigIds;

	Graph(std::unordered_set<std::string> contigIds) : contigIds(contigIds) {}

	void insertOverlap(Overlap& overlap);

	std::vector<Path> constructPaths(std::string start);

private:
	std::unordered_map<std::string, Node> nodes;

	bool finishPath(Path& path);
};