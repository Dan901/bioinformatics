#include "overlap.h"
#include <vector>
#include <string>

Overlap::Overlap(std::vector<std::string> elements) {
	queryId = elements[0];
	queryLen = std::stoi(elements[1]);
	queryStart = std::stoi(elements[2]);
	queryEnd = std::stoi(elements[3]);
	strand = elements[4] == "+";
	targetId = elements[5];
	targetLen = std::stoi(elements[6]);
	targetStart = std::stoi(elements[7]);
	targetEnd = std::stoi(elements[8]);
	overlapScore = calculateOverlapScore((double)std::stoi(elements[9]) / std::stoi(elements[10]));
	extensionScore1 = calculateExtensionScore1();
	extensionScore2 = calculateExtensionScore2();
}

double Overlap::calculateOverlapScore(double si) {
	double ol1 = queryEnd - queryStart;
	double ol2 = targetEnd - targetStart;
	return (ol1 + ol2) * si / 2;
}

double Overlap::calculateExtensionScore1() {
	double oh1 = queryLen - queryEnd - 1;
	double oh2 = targetStart;
	double el2 = targetLen - targetEnd - 1;
	return overlapScore + el2 / 2 - (oh1 + oh2) / 2;
}

double Overlap::calculateExtensionScore2() {
	double oh1 = queryStart;
	double oh2 = targetLen - targetEnd - 1;
	double el1 = queryLen - queryEnd - 1;
	return overlapScore + el1 / 2 - (oh1 + oh2) / 2;
}


