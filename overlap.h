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
	
	bool strand;
	double overlapScore;
	double extensionScore;
	int extensionLen;
	int overhangLen;

	Overlap(PafLine& pafLine);
};

bool compareByOverlapScore(Overlap* o1, Overlap* o2);
bool compareByExtensionScore(Overlap* o1, Overlap* o2);