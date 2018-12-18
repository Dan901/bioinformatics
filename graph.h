#include <string>
#include <vector>
#include <unordered_map>
#include "overlap.h"

struct Node {
	std::vector<Overlap> rightExtensions;
	std::vector<Overlap> leftExtensions;
};

struct Graph {
	std::unordered_map<std::string, Node> contigs;
	std::unordered_map<std::string, Node> reads;
};