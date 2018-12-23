#include "graph.h"

void Graph::insertOverlap(Overlap & overlap) {
	nodes[overlap.leftId].extensions.push_back(overlap);
}

std::vector<Path> Graph::constructPaths(std::string start) {
	std::vector<Path> paths;

	for (auto overlap : nodes[start].extensions) {
		Path path(start);
		path.extend(overlap);

		if (finishPath(path)) {
			paths.push_back(std::move(path));
		}
	}

	return paths;
}

// try to extend the path to another contig
bool Graph::finishPath(Path & path) {


	return false;
}