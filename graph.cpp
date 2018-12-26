#include <algorithm>
#include <iostream>
#include "graph.h"

long MAX_PATH_LEN = 2000000;

void Graph::insertOverlap(Overlap & overlap) {
	extensions[overlap.leftId].push_back(overlap);
}

// get all paths from given starting node
std::vector<Path> Graph::constructPaths(std::string start) {
	std::vector<Path> paths;

	for (auto extensionSelector : extensionSelectors) {
		for (auto first : extensions[start]) {
			try {
				Path path = dfs(&first, extensionSelector);

				if (!path.overlaps.empty()) {
					path.populateReads();
					paths.push_back(std::move(path));
				}
			} catch (PathTooLongException& e) {
				continue;
			}
		}
	}

	return paths;
}

// construct path using DFS starting with given overlap
Path Graph::dfs(Overlap * first, ExtensionSelector * extensionSelector) {
	Path path(first->leftId, first->leftLen);
	path.add(first);

	std::unordered_set<std::string> visitedNodes;
	visitedNodes.insert(first->leftId);
	visitedNodes.insert(first->rightId);

	while (!path.overlaps.empty()) {
		if (path.length > MAX_PATH_LEN) {
			throw PathTooLongException();
		}
		
		std::string current = path.overlaps.back()->rightId;
		if (contigIds.find(current) != contigIds.end()) {
			return path;
		}

		Overlap* next = extensionSelector->getNextExtension(extensions[current], visitedNodes);
		if (next == nullptr) {
			path.removeLast();
			continue;
		}

		path.add(next);
		visitedNodes.insert(next->rightId);
	}

	return path;
}