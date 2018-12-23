#include <string>
#include <vector>

class PafLine {
public:
	std::string queryId;
	int queryLen;
	int queryStart;
	int queryEnd;
	bool strand;
	std::string targetId;
	int targetLen;
	int targetStart;
	int targetEnd;
	double overlapScore;
	double extensionScore1; // target udesno od querija
	double extensionScore2; // target ulijevo od querija

	PafLine(std::vector<std::string> elements);

	bool isFullyContained();
	bool isTargetExtendingRight();
	double getExtensionScore();

private:
	double calculateOverlapScore(double si);
	double calculateExtensionScore1();
	double calculateExtensionScore2();
};