#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>

#include "overlap.h"

std::vector<std::string> split(std::string line, char delim) {
	std::vector<std::string> elements;
	std::stringstream ss(line);
	std::string item;

	while (std::getline(ss, item, delim)) {
		elements.push_back(item);
	}

	return elements;
}

int main() {
	std::ifstream input("C:\\FER - local\\Bioinformatika\\data\\BGrahamii - real\\a.txt");
	std::string line;
	std::vector<Overlap> overlaps;

	while (std::getline(input, line)) {
		overlaps.emplace_back(split(line, '\t'));
	}

	for (auto overlap : overlaps) {
		std::cout << overlap.overlapScore << '\t' << overlap.extensionScore1 << '\t' << overlap.extensionScore2 << '\n';
	}

	input.close();
	return 0;
}