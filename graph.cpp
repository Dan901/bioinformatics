#include <algorithm>
#include <iostream>
#include "graph.h"

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
	e1.sequenceId = line.sequenceId;

	e2.nextId = line.queryId;
	e2.nextLen = line.queryLen;
	e2.nextStart = line.queryStart;
	e2.nextEnd = line.queryEnd;
	e2.lastLen = line.targetLen;
	e2.lastStart = line.targetStart;
	e2.lastEnd = line.targetEnd;
	e2.overlapScore = line.overlapScore;
	e2.sameStrand = line.sameStrand;
	e2.sequenceId = line.sequenceId;

	if (line.extensionScore1 > line.extensionScore2) {
		e1.extensionScore = line.extensionScore1;
		e1.extensionLen = line.lengths[3];
		e1.overhangLen = line.lengths[1];

		e2.extensionScore = line.extensionScore3;
		e2.extensionLen = line.lengths[0];
		e2.overhangLen = line.lengths[2];
	} else {
		e1.extensionScore = line.extensionScore4;
		e1.extensionLen = line.lengths[2];
		e1.overhangLen = line.lengths[0];

		e2.extensionScore = line.extensionScore2;
		e2.extensionLen = line.lengths[1];
		e2.overhangLen = line.lengths[3];
	}

	//if (e1.overhangLen >= MAX_OVERHANG || e2.overhangLen >= MAX_OVERHANG) return;
	//if (e1.extensionLen < MIN_EXTENSION || e2.extensionLen < MIN_EXTENSION) return;
	int overhangSum = e1.overhangLen + e2.overhangLen;
	if (overhangSum > MAX_OVERHANG_EXTENSION_RATIO * e1.extensionLen || overhangSum > MAX_OVERHANG_EXTENSION_RATIO * e2.extensionLen) return;

	if (line.sameStrand) {
		if (line.extensionScore1 > line.extensionScore2) {
			suffixes[line.queryId].push_back(e1);
			prefixes[line.targetId].push_back(e2);
		} else {
			prefixes[line.queryId].push_back(e1);
			suffixes[line.targetId].push_back(e2);
		}
	} else {
		if (line.extensionScore1 > line.extensionScore2) {
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
				if (path.extensions.empty()) continue;

				path.finishPath();
				paths.push_back(std::move(path));
			} catch (PathTooLongException& e) {
				continue;
			}
		}
	}

	for (int i = 0; i < RANDOM_PATH_TRIALS; i++) {
		try {
			Path path = randomPath(start);
			if (path.extensions.empty() || contigIds.find(path.extensions.back()->nextId) == contigIds.end()) continue;

			path.finishPath();
			paths.push_back(std::move(path));
		} catch (PathTooLongException& e) {
			continue;
		}
	}

	return paths;
}

// construct path using DFS starting with given overlap
Path Graph::dfs(std::string start, Extension * first, ExtensionSelector * extensionSelector) {
	bool direction = first->sameStrand;

	Path path(start);
	path.add(first);

	std::unordered_set<std::string> visitedNodes;
	visitedNodes.insert(start);
	visitedNodes.insert(first->nextId);

	while (!path.extensions.empty()) {
		std::string current = path.extensions.back()->nextId;
		std::vector<Extension>& candidates = direction ? suffixes[current] : prefixes[current];

		Extension* next = findExtensionToContig(candidates, start);
		if (next) {
			path.add(next);
			return path;
		}

		if (path.length > MAX_PATH_LEN) {
			throw PathTooLongException();
		}

		next = extensionSelector->getNextExtension(candidates, visitedNodes);

		if (next) {
			visitedNodes.insert(next->nextId);
			path.add(next);

			if (!next->sameStrand) {
				direction = !direction;
			}

			continue;
		}

		path.removeLast();
		direction = getNextDirection(path);
	}

	return path;
}

Path Graph::randomPath(std::string start) {
	Path path(start);
	bool direction = true;
	std::string current = start;

	std::unordered_set<std::string> visitedNodes;
	visitedNodes.insert(start);

	while (true) {
		std::vector<Extension>& candidates = direction ? suffixes[current] : prefixes[current];

		Extension* next = findExtensionToContig(candidates, start);
		if (next) {
			path.add(next);
			return path;
		}

		if (path.length > MAX_PATH_LEN) {
			throw PathTooLongException();
		}

		next = getRandomExtension(candidates, visitedNodes);
		if (!next) return path;

		path.add(next);
		current = next->nextId;
		visitedNodes.insert(current);

		if (!next->sameStrand) {
			direction = !direction;
		}
	}
}

Extension * Graph::getRandomExtension(std::vector<Extension>& extensions, std::unordered_set<std::string>& visitedNodes) {
	std::vector<Extension*> candidates;
	for (auto & e : extensions) {
		if (visitedNodes.find(e.nextId) != visitedNodes.end()) continue;
		candidates.push_back(&e);
	}

	if (candidates.empty()) return nullptr;

	double scoreSum = 0;
	for (auto * e : candidates) {
		scoreSum += e->extensionScore;
	}

	std::uniform_real_distribution<double> unif(0, scoreSum);
	double random = unif(randomEngine);

	for (auto * e : candidates) {
		if (random <= e->extensionScore) return e;
		random -= e->extensionScore;
	}

	return candidates.back();
}

bool Graph::getNextDirection(Path & path) {
	bool direction = true;

	for (auto e : path.extensions) {
		if (!e->sameStrand) {
			direction = !direction;
		}
	}

	return direction;
}

// returns extension to contig if it exists or nullptr
Extension * Graph::findExtensionToContig(std::vector<Extension>& extensions, std::string start) {
	for (auto & e : extensions) {
		if (e.nextId == start) continue;
		if (contigIds.find(e.nextId) != contigIds.end()) {
			return &e;
		}
	}

	return nullptr;
}
