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

	Overlap(PafLine& pafLine);
};