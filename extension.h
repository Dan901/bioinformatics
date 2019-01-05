#include <string>

struct Extension {
	std::string nextId;
	int nextLen;
	int nextStart;
	int nextEnd;

	int lastLen;
	int lastStart;
	int lastEnd;

	bool sameStrand;

	double overlapScore;
	double extensionScore;
	int extensionLen;
	int overhangLen;
};

bool compareByOverlapScore(Extension* e1, Extension* e2);
bool compareByExtensionScore(Extension* e1, Extension* e2);