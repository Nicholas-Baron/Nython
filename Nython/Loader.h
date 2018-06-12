#ifndef _LOADER
#define _LOADER

#include "Keywords.h"

#include <fstream>
#include <string>
#include <vector>

std::vector<std::string> fileContents(const std::string& location);

inline bool isValidFile(const std::string& location) {

	std::ifstream file(location.c_str());
	bool error = !file;
	file.close();
	return !error;
}

std::vector<Token*> tokens(std::vector<std::string> program);

inline std::string trim(const std::string& str_in, const std::string& whitespace = " \t") {
	const auto strBegin = str_in.find_first_not_of(whitespace);
	if(strBegin == std::string::npos) {
		return "";
	}
	const auto strEnd = str_in.find_last_not_of(whitespace);
	const auto strRange = strEnd - strBegin;

	return str_in.substr(strBegin, strRange + 1);
}

inline bool hasComment(const std::string& str_in) { return str_in.find_first_of("//") != std::string::npos; }

std::vector<std::string> split(const std::string& str, const std::string& delim);

std::string spaceDelimsOps(const std::string& line);

#endif // !_LOADER

