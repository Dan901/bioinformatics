#include <algorithm>
#include <iostream>
#include "graph.h"

long MAX_PATH_LEN = 5000000;

void Graph::insertOverlap(Overlap & overlap) {
	rightExtensions[overlap.leftId].push_back(overlap);
	leftExtensions[overlap.rightId].push_back(overlap);
}

// get all paths from given starting node
std::vector<Path> Graph::constructPaths(std::string start) {
	std::vector<Path> paths;

	for (auto extensionSelector : extensionSelectors) {
		
		for (std::vector<Overlap>::iterator first = rightExtensions[start].begin(); first != rightExtensions[start].end(); ++first) {
			try {
				Path path = dfs(&(*first), extensionSelector);

				if (!path.overlaps.empty()) {
					path.finishPath();
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
	bool direction = first->sameStrand;

	Path path(first->leftId, first->leftLen);
	path.add(first, direction);

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

		Overlap* next = extensionSelector->getNextExtension(direction ? rightExtensions[current] : leftExtensions[current], visitedNodes);
		if (!next) {
			bool lastDirection = path.removeLast(direction);
			if (!lastDirection) {
				direction = !direction;
			}
			continue;
		}

		if (!next->sameStrand) {
			direction = !direction;
		}
		path.add(next, direction);
		visitedNodes.insert(next->rightId);
	}

	return path;
}