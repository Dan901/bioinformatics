#include "path.h"
#include <cmath>

void Path::add(Extension * extension) {
	length -= extension->overhangLen;
	length += extension->extensionLen;

	extensions.push_back(extension);
}

void Path::removeLast() {
	Extension* last = extensions.back();
	length += last->overhangLen;
	length -= last->extensionLen;

	extensions.pop_back();
}

void Path::finishPath() {
	reads.push_back(start);
	std::vector<double> sequenceIds;
	for (auto e : extensions) {
		sequenceIds.push_back(e->sequenceId);
		averageSeqId += e->sequenceId;
		reads.push_back(e->nextId);
	}
	averageSeqId /= extensions.size();
	std::sort(sequenceIds.begin(), sequenceIds.end());

	if ((sequenceIds.size() + 1) % 2 == 0) {
		medianSeqId = sequenceIds[(sequenceIds.size() + 1) / 2 - 1];
	}
	else {
		double index = floor((sequenceIds.size() + 1) / 2) - 1;
		medianSeqId = (sequenceIds[index] + sequenceIds[index + 1]) / 2;
	}

	length += extensions.front()->lastLen;
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
