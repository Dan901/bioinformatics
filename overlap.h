#include <string>
#include <vector>

class Overlap {
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
	double extensionScore1; // query extends target
	double extensionScore2; // target extends query

	Overlap(std::vector<std::string> elements);

private:
	double calculateOverlapScore(double si);
	double calculateExtensionScore1();
	double calculateExtensionScore2();
};