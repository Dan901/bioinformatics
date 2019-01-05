#include <string>
#include <vector>
#include <unordered_set>
#include "extension.h"

#include <iostream>

struct Path {
	std::string start;
	long length;
	std::vector<Extension*> extensions;
	std::vector<std::string> reads;

	Path() {}
	Path(std::string start, long length) : start(start), length(length) {}

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