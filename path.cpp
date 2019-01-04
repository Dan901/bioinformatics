#include "path.h"

void Path::add(Overlap * overlap, bool direction) {
	length -= overlap->getOverhangLen(direction);
	length += overlap->getExtensionLen(direction);

	overlaps.push_back(overlap);
}

bool Path::removeLast(bool direction) {
	Overlap* last = overlaps.back();
	bool lastDirection = last->sameStrand;

	length -= last->getExtensionLen(direction);
	length += last->getOverhangLen(direction);

	overlaps.pop_back();
	return lastDirection;
}

void Path::finishPath() {
	reads.push_back(start);
	for (auto o : overlaps) {
		reads.push_back(o->rightId);
	}
}

bool PathComparator::operator()(const Path & obj1, const Path & obj2) const {
	return obj1.reads == obj2.reads;
}

size_t PathHasher::operator()(const Path & obj) const {
	std::hash<std::string> hasher;
	size_t seed = obj.reads.size();

	for (auto r : obj.reads) {
		seed ^= hasher(r) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

	return seed;
}
