#include <string>
#include <vector>

class PafLine {
public:
	std::string queryId;
	int queryLen;
	int queryStart;
	int queryEnd;
	bool sameStrand;
	std::string targetId;
	int targetLen;
	int targetStart;
	int targetEnd;

	PafLine(std::vector<std::string> elements);

	bool isFullyContained();
	bool isTargetExtendingRight();
	double getOverlapScore();
	double getExtensionScore();
	int getExtensionLenToRight();
	int getExtensionLenToLeft();
	int getOverhangLenToRight();
	int getOverhangLenToLeft();

private:
	int lengths[4];

	double overlapScore;
	double extensionScore1; // target is right of query
	double extensionScore2; // target is left of query

	double calculateOverlapScore(double si);
	double calculateExtensionScore1();
	double calculateExtensionScore2();
};