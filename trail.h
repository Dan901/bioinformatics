#include <string>
#include <vector>

class Trail{
public:
    std::vector<std::pair<std::string, std::string>> trail;
    double goodness; 
    std::string getName();
};