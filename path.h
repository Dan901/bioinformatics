#include <string>
#include <vector>
#include <unordered_set>
#include "overlap.h"

class Path {
public:
	std::string start;
	std::string end;
	long length;

	std::vector<PafLine> overlaps;
	std::unordered_set<std::string> reads;

	Path(std::string start) : start(start) {}

	void extend(Overlap& overlap);
};