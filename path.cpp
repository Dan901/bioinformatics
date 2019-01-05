#include "path.h"

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
	for (auto e : extensions) {
		reads.push_back(e->nextId);
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
