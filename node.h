#include <string>
#include <vector>
#include <map>

class Node{
public:
    std::string contigId;
    // contigs and its value
    std::map<std::string, double> vertices; 
};