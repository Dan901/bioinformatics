#include "paf.h"

class Overlap {
public:
	std::string leftId;
	int leftLen;
	int leftStart;
	int leftEnd;
	
	std::string rightId;
	int rightLen;
	int rightStart;
	int rightEnd;
	
	bool sameStrand;
	double overlapScore;
	double extensionScore;

	Overlap(PafLine& pafLine);

	int getExtensionLen(bool direction);
	int getOverhangLen(bool direction);

private:
	int extensionLenToRight;
	int extensionLenToLeft;
	int overhangLenToRight;
	int overhangLenToLeft;
};

bool compareByOverlapScore(Overlap* o1, Overlap* o2);
bool compareByExtensionScore(Overlap* o1, Overlap* o2);