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
	} else {
		leftId = pafLine.targetId;
		leftLen = pafLine.targetLen;
		leftStart = pafLine.targetStart;
		leftEnd = pafLine.targetEnd;
		rightId = pafLine.queryId;
		rightLen = pafLine.queryLen;
		rightStart = pafLine.queryStart;
		rightEnd = pafLine.queryEnd;
	}

	sameStrand = pafLine.sameStrand;
	overlapScore = pafLine.getOverlapScore();
	extensionScore = pafLine.getExtensionScore();
	extensionLenToRight = pafLine.getExtensionLenToRight();
	extensionLenToLeft = pafLine.getExtensionLenToLeft();
	overhangLenToRight = pafLine.getOverhangLenToRight();
	overhangLenToLeft = pafLine.getOverhangLenToLeft();
}

int Overlap::getExtensionLen(bool direction) {
	return direction ? extensionLenToRight : extensionLenToLeft;
}

int Overlap::getOverhangLen(bool direction) {
	return direction ? overhangLenToRight : overhangLenToLeft;
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
