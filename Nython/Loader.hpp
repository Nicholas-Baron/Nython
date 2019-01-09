#ifndef _LOADER
#define _LOADER

#include "Keywords.hpp"

#include <fstream>
#include <string>
#include <vector>
#include <memory>

namespace Loader {
	std::vector<std::string> fileContents(const std::string& location);

	inline bool isValidFile(const std::string& location) {
		std::ifstream file(location.c_str());
		const bool present = static_cast<bool>(file);
		file.close();
		return present;
	}

	std::vector<std::shared_ptr<const Token>> tokens(const std::vector<std::string>& program);

	inline std::string trim(const std::string& str_in, const std::string& whitespace = " \t") {
		const size_t strBegin = str_in.find_first_not_of(whitespace);

		if(strBegin == std::string::npos) {
			const std::string empty;
			return empty;
		}

		const size_t strEnd = str_in.find_last_not_of(whitespace);
		const size_t strRange = strEnd - strBegin;

		return str_in.substr(strBegin, strRange + 1);
	}

	inline bool hasComment(const std::string& str_in, unsigned loc) noexcept {
		return str_in.find("//", loc) != std::string::npos;
	}

	std::vector<std::string> splitIntoTokens(const std::string& str, const std::string& delim);

	std::string spaceDelimsOps(const std::string& line);
}
#endif // !_LOADER
