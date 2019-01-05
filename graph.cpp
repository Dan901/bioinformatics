#include <algorithm>
#include <iostream>
#include "graph.h"

long MAX_PATH_LEN = 5000000;

void Graph::insertExtensions(PafLine & line) {
	Extension e1, e2;

	e1.nextId = line.targetId;
	e1.nextLen = line.targetLen;
	e1.nextStart = line.targetStart;
	e1.nextEnd = line.targetEnd;
	e1.lastLen = line.queryLen;
	e1.lastStart = line.queryStart;
	e1.lastEnd = line.queryEnd;
	e1.overlapScore = line.overlapScore;
	e1.sameStrand = line.sameStrand;

	e2.nextId = line.queryId;
	e2.nextLen = line.queryLen;
	e2.nextStart = line.queryStart;
	e2.nextEnd = line.queryEnd;
	e2.lastLen = line.targetLen;
	e2.lastStart = line.targetStart;
	e2.lastEnd = line.targetEnd;
	e2.overlapScore = line.overlapScore;
	e2.sameStrand = line.sameStrand;

	if (line.extensionScore1 > line.extensionScore2) {
		e1.extensionScore = line.extensionScore1;
		e1.extensionLen = line.lengths[3];
		e1.overhangLen = line.lengths[1];

		e2.extensionScore = line.extensionScore3;
		e2.extensionLen = line.lengths[0];
		e2.overhangLen = line.lengths[2];

		if (line.sameStrand) {
			suffixes[line.queryId].push_back(e1);
			prefixes[line.targetId].push_back(e2);
		} else {
			prefixes[line.queryId].push_back(e1);
			suffixes[line.targetId].push_back(e2);
		}
	} else {
		e1.extensionScore = line.extensionScore4;
		e1.extensionLen = line.lengths[2];
		e1.overhangLen = line.lengths[0];

		e2.extensionScore = line.extensionScore2;
		e2.extensionLen = line.lengths[1];
		e2.overhangLen = line.lengths[3];

		if (line.sameStrand) {
			suffixes[line.queryId].push_back(e1);
			suffixes[line.targetId].push_back(e2);
		} else {
			prefixes[line.queryId].push_back(e1);
			prefixes[line.targetId].push_back(e2);
		}
	}
}

// get all paths from given starting node
std::vector<Path> Graph::constructPaths(std::string start) {
	std::vector<Path> paths;

	for (auto extensionSelector : extensionSelectors) {
		
		for (std::vector<Extension>::iterator first = suffixes[start].begin(); first != suffixes[start].end(); ++first) {
			try {
				Path path = dfs(start, &(*first), extensionSelector);

				if (!path.extensions.empty()) {
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
Path Graph::dfs(std::string start, Extension * first, ExtensionSelector * extensionSelector) {
	bool direction = first->sameStrand;

	Path path(start, first->lastLen);
	path.add(first);

	std::unordered_set<std::string> visitedNodes;
	visitedNodes.insert(start);
	visitedNodes.insert(first->nextId);

	while (!path.extensions.empty()) {
		if (path.length > MAX_PATH_LEN) {
			throw PathTooLongException();
		}
		
		std::string current = path.extensions.back()->nextId;
		if (contigIds.find(current) != contigIds.end()) {
			return path;
		}

		Extension* next = extensionSelector->getNextExtension(direction ? suffixes[current] : prefixes[current], visitedNodes);
		
		if (!next) {
			path.removeLast();
			direction = getNextDirection(path);
			continue;
		}

		visitedNodes.insert(next->nextId);
		path.add(next);
		if (direction != next->sameStrand) {
			direction = !direction;
		}
	}

	return path;
}

bool Graph::getNextDirection(Path & path) {
	bool direction = true;

	for (auto e : path.extensions) {
		if (direction != e->sameStrand) {
			direction = !direction;
		}
	}

	return direction;
}
