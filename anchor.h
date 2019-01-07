#include <string>
#include <vector>

class Anchor{
public:
    std::string contig;

    std::string firstBestContig;
    std::string secondBestContig;;
    int firstNP = -1;
    int secondNP = -1;

    double conflictIndex = -1.0;

    void calculateConflictIndex();
};