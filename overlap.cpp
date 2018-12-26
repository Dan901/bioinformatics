#include "overlap.h"

Overlap::Overlap(PafLine & pafLine) {
	if (pafLine.isTargetExtendingRight()) {
		leftId = pafLine.queryId;
		leftLen = pafLine.queryLen;
		leftStart = pafLine.queryStart;
		leftEnd = pafLine.queryEnd;
		rightId = pafLine.targetId;
		rightLen = pafLine.targetLen;
		rightStart = pafLine.targetStart;
		rightEnd = pafLine.targetEnd;
		extensionScore = pafLine.extensionScore1;
		extensionLen = pafLine.targetLen - pafLine.targetEnd - 1;
		overhangLen = pafLine.queryLen - pafLine.queryEnd - 1; // TODO: possibly -1
	} else {
		leftId = pafLine.targetId;
		leftLen = pafLine.targetLen;
		leftStart = pafLine.targetStart;
		leftEnd = pafLine.targetEnd;
		rightId = pafLine.queryId;
		rightLen = pafLine.queryLen;
		rightStart = pafLine.queryStart;
		rightEnd = pafLine.queryEnd;
		extensionScore = pafLine.extensionScore2;
		extensionLen = pafLine.targetStart;
		overhangLen = pafLine.queryStart;
	}

	strand = pafLine.strand;
	overlapScore = pafLine.overlapScore;
}

// true if o2 is better than o1
bool compareByOverlapScore(Overlap * o1, Overlap * o2) {
	if (o1->overlapScore == o2->overlapScore) {
		return o2->rightLen > o1->rightLen;
	}

	return o2->overlapScore > o1->overlapScore;
}

// true if o2 is better than o1
bool compareByExtensionScore(Overlap * o1, Overlap * o2) {
	if (o1->overlapScore == o2->overlapScore) {
		return o2->rightLen > o1->rightLen;
	}

	return o2->extensionScore > o1->extensionScore;
}
