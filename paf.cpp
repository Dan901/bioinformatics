#include "paf.h"
#include <vector>
#include <string>

PafLine::PafLine(std::vector<std::string> elements) {
	queryId = elements[0];
	queryLen = std::stoi(elements[1]);
	queryStart = std::stoi(elements[2]);
	queryEnd = std::stoi(elements[3]);
	sameStrand = elements[4] == "+";
	targetId = elements[5];
	targetLen = std::stoi(elements[6]);
	targetStart = std::stoi(elements[7]);
	targetEnd = std::stoi(elements[8]);
	sequenceId = (double)std::stoi(elements[9]) / std::stoi(elements[10]);
	overlapScore = calculateOverlapScore();

	lengths[0] = queryStart;
	lengths[1] = queryLen - queryEnd - 1;
	lengths[2] = sameStrand ? targetStart : targetLen - targetEnd - 1;
	lengths[3] = sameStrand ? targetLen - targetEnd - 1 : targetStart;

	extensionScore1 = calculateExtensionScore1();
	extensionScore2 = calculateExtensionScore2();
	extensionScore3 = calculateExtensionScore3();
	extensionScore4 = calculateExtensionScore4();
}

bool PafLine::isFullyContained() {
	return ((lengths[0] >= lengths[2]) && (lengths[1] >= lengths[3])) ||
		((lengths[2] >= lengths[0]) && (lengths[3] >= lengths[1]));
}

double PafLine::calculateOverlapScore() {
	double ol1 = queryEnd - queryStart;
	double ol2 = targetEnd - targetStart;
	return (ol1 + ol2) * sequenceId / 2;
}

double PafLine::calculateExtensionScore1() {
	double oh1 = lengths[1];
	double oh2 = lengths[2];
	double el = lengths[3];
	return overlapScore + el / 2 - (oh1 + oh2) / 2;
}

double PafLine::calculateExtensionScore2() {
	double oh1 = lengths[0];
	double oh2 = lengths[3];
	double el = lengths[1];
	return overlapScore + el / 2 - (oh1 + oh2) / 2;
}

double PafLine::calculateExtensionScore3() {
	double oh1 = lengths[1];
	double oh2 = lengths[2];
	double el = lengths[0];
	return overlapScore + el / 2 - (oh1 + oh2) / 2;
}

double PafLine::calculateExtensionScore4() {
	double oh1 = lengths[0];
	double oh2 = lengths[3];
	double el = lengths[2];
	return overlapScore + el / 2 - (oh1 + oh2) / 2;
}