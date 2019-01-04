#include <string>
#include <vector>
#include <unordered_set>
#include "overlap.h"

#include <iostream>

struct Path {
	std::string start;
	long length;
	std::vector<Overlap*> overlaps;
	std::vector<std::string> reads;

	Path() {}
	Path(std::string start, long length) : start(start), length(length) {}

	void add(Overlap* overlap, bool direction);
	bool removeLast(bool direction);
	void finishPath();
};

struct PathComparator {
	bool operator()(const Path& obj1, const Path& obj2) const;
};

struct PathHasher {
	size_t operator()(const Path& obj) const;
};