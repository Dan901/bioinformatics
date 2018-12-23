#include "graph.h"

void Graph::insertReadContigOverlap(Overlap & overlap) {
	contigIds.insert(overlap.queryId);

	insertOverlap(overlap);
}

void Graph::insertOverlap(Overlap & overlap) {
	if (overlap.isTargetExtendingRight()) {
		nodes[overlap.queryId].rightExtensions.push_back(overlap);
		nodes[overlap.targetId].leftExtensions.push_back(overlap);
	} else {
		nodes[overlap.queryId].leftExtensions.push_back(overlap);
		nodes[overlap.targetId].rightExtensions.push_back(overlap);
	}
}