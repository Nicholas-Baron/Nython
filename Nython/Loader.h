#ifndef _LOADER
#define _LOADER

#include "Keywords.h"

#include <fstream>
#include <string>
#include <vector>

namespace Loader {

	std::vector<std::string> fileContents(const std::string& location);

	inline bool isValidFile(const std::string& location) {

		std::ifstream file(location.c_str());
		const auto error = !file;
		file.close();
		return !error;
	}

	std::vector<Token*> tokens(const std::vector<std::string>& program);

	inline std::string trim(const std::string& str_in, const std::string& whitespace = " \t") {
		const auto strBegin = str_in.find_first_not_of(whitespace);
		if(strBegin == std::string::npos) {
			return "";
		}
		const auto strEnd = str_in.find_last_not_of(whitespace);
		const auto strRange = strEnd - strBegin;

		return str_in.substr(strBegin, strRange + 1);
	}

	inline bool hasComment(const std::string& str_in, unsigned& loc) {
		for(unsigned i = loc; i < str_in.size(); i++) {
			const char& c = str_in[i];
			if(c == '/') {
				for(unsigned j = i + 1; j < str_in.size(); j++) {
					if(str_in[j] == '/') {
						return true;
					} else if(str_in[j] != ' ') {
						j = str_in.size();
					}
				}
			}

		}

		return false;
	}

	std::vector<std::string> splitIntoTokens(const std::string& str, const std::string& delim);

	std::string spaceDelimsOps(const std::string& line);
}
#endif // !_LOADER

