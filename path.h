#include <string>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include "extension.h"

#include <iostream>

struct Path {
	std::string start;
	bool extendingTail;
	long length = 0;
	std::vector<Extension*> extensions;
	std::vector<std::string> reads;
	double averageSeqId = 0;
	double medianSeqId = 0;

	Path() {}
	Path(std::string start, bool extendingTail) : start(start), extendingTail(extendingTail) {}

	void add(Extension* extension);
	void removeLast();
	void finishPath();
};

struct PathComparator {
	bool operator()(const Path& obj1, const Path& obj2) const;
};

struct PathHasher {
	size_t operator()(const Path& obj) const;
};