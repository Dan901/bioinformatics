#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "path.h"

struct PathTooLongException : public std::exception {};

class ExtensionSelector {
public:
	virtual Overlap* getNextExtension(std::vector<Overlap>& extensions, std::unordered_set<std::string>& visitedNodes) = 0;
};

class Graph {
public:
	std::unordered_set<std::string> contigIds;
	std::vector<ExtensionSelector*> extensionSelectors;

	Graph(std::unordered_set<std::string> contigIds, std::vector<ExtensionSelector*> extensionSelectors) : contigIds(contigIds), extensionSelectors(extensionSelectors) {}

	void insertOverlap(Overlap& overlap);
	std::vector<Path> constructPaths(std::string start);

private:
	std::unordered_map<std::string, std::vector<Overlap>> extensions;

	Path dfs(Overlap* first, ExtensionSelector* extensionSelector);
};

template <typename Compare>
class BestExtensionSelector : public ExtensionSelector {
public:
	Compare compare;

	BestExtensionSelector(Compare compare) : compare(compare) {}

	virtual Overlap* getNextExtension(std::vector<Overlap>& extensions, std::unordered_set<std::string>& visitedNodes);
};

template<typename Compare>
inline Overlap * BestExtensionSelector<Compare>::getNextExtension(std::vector<Overlap> & extensions, std::unordered_set<std::string> & visitedNodes) {
	Overlap* best = nullptr;

	for (auto & o : extensions) {
		if (visitedNodes.find(o.rightId) != visitedNodes.end()) continue;
		if (best == nullptr || compare(best, &o)) {
			best = &o;
		}
	}

	return best;
}
