#include "extension.h"

// true if e2 is better than e1
bool compareByOverlapScore(Extension * e1, Extension * e2) {
	if (e1->overlapScore == e2->overlapScore) {
		return e2->nextLen > e1->nextLen;
	}

	return e2->overlapScore > e1->overlapScore;
}

// true if e2 is better than e1
bool compareByExtensionScore(Extension * e1, Extension * e2) {
	if (e2->extensionScore == e1->extensionScore) {
		return e2->nextLen > e1->nextLen;
	}

	return e2->extensionScore > e1->extensionScore;
}
