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

	int lengths[4];

	double overlapScore;
	double extensionScore1;
	double extensionScore2;
	double extensionScore3;
	double extensionScore4;

	PafLine(std::vector<std::string> elements);

	bool isFullyContained();
	double getBiggerExtensionScore();

private:
	double calculateOverlapScore(double si);
	double calculateExtensionScore1();
	double calculateExtensionScore2();
	double calculateExtensionScore3();
	double calculateExtensionScore4();
};