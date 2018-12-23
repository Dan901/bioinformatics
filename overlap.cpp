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
	}

	strand = pafLine.strand;
	overlapScore = pafLine.overlapScore;
}
