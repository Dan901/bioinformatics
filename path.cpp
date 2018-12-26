#include "path.h"

void Path::add(Overlap * overlap) {
	length -= overlap->overhangLen;
	length += overlap->extensionLen;

	overlaps.push_back(overlap);
}

void Path::removeLast() {
	Overlap* last = overlaps.back();
	length -= last->extensionLen;
	length += last->overhangLen;

	overlaps.pop_back();
}

void Path::populateReads() {
	reads.push_back(start);
	for (auto o : overlaps) {
		reads.push_back(o->rightId);
	}
}

bool PathComparator::operator()(const Path & obj1, const Path & obj2) const {
	return std::equal(obj1.reads.begin(), obj1.reads.end(), obj2.reads.begin());
}

size_t PathHasher::operator()(const Path & obj) const {
	std::hash<std::string> hasher;
	size_t seed = obj.reads.size();

	for (auto r : obj.reads) {
		seed ^= hasher(r) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

	return seed;
}
