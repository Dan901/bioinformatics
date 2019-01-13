#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include <ctime>
#include "paf.h"
#include "path.h"

struct PathTooLongException : public std::exception {};

class ExtensionSelector {
public:
	virtual Extension* getNextExtension(std::vector<Extension>& extensions, std::unordered_set<std::string>& visitedNodes) = 0;
};

class Graph {
public:
	std::unordered_set<std::string> contigIds;
	std::vector<ExtensionSelector*> extensionSelectors;

	Graph(std::unordered_set<std::string> contigIds, std::vector<ExtensionSelector*> extensionSelectors) : contigIds(contigIds), extensionSelectors(extensionSelectors), randomEngine(std::default_random_engine(time(0))) {}

	void insertExtensions(PafLine& line);
	std::vector<Path> constructPaths(std::string start);

private:
	const long MAX_PATH_LEN = 300000;
	const double MAX_OVERHANG_EXTENSION_RATIO = 0.15;
	const int RANDOM_PATH_TRIALS = 200;

	std::default_random_engine randomEngine;
	std::unordered_map<std::string, std::vector<Extension>> prefixes;
	std::unordered_map<std::string, std::vector<Extension>> suffixes;

	Path dfs(std::string start, Extension* first, ExtensionSelector* extensionSelector);
	Path randomPath(std::string start);
	Extension* getRandomExtension(std::vector<Extension>& extensions, std::unordered_set<std::string>& visitedNodes);
	bool getNextDirection(Path& path);
	Extension* findExtensionToContig(std::vector<Extension> & extensions, std::string start);
};

template <typename Compare>
class BestExtensionSelector : public ExtensionSelector {
public:
	Compare compare;

	BestExtensionSelector(Compare compare) : compare(compare) {}

	virtual Extension* getNextExtension(std::vector<Extension>& extensions, std::unordered_set<std::string>& visitedNodes);
};

template<typename Compare>
inline Extension * BestExtensionSelector<Compare>::getNextExtension(std::vector<Extension> & extensions, std::unordered_set<std::string> & visitedNodes) {
	Extension* best = nullptr;

	for (auto & e : extensions) {
		if (visitedNodes.find(e.nextId) != visitedNodes.end()) continue;

		if (best == nullptr || compare(best, &e)) {
			best = &e;
		}
	}

	return best;
}