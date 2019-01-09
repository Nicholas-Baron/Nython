#include "Loader.hpp"

#include <iostream>
#include <sstream>
#include <memory>

//Loads a file line by line
std::vector<std::string> Loader::fileContents(const std::string& location) {
	std::vector<std::string> toRet;
	std::ifstream file(location.c_str());

	if(!file) {
		toRet.push_back("[ERROR]");
		return toRet;
	}

	std::string current;

	while(getline(file, current)) {
		toRet.push_back(current);
	}

	file.close();

	return toRet;
}

//Breaks the string into its tokens
std::vector<std::shared_ptr<const Token>> Loader::tokens(const std::vector<std::string>& program) {
	std::vector<std::shared_ptr<const Token>> toRet;

	for(unsigned line = 0; line < program.size(); line++) {
		bool isComment = false;
		auto whole_line = trim(program.at(line));

		if(hasComment(whole_line, 0)) {
			std::cout << "Comment found on line #" << line << std::endl;
		}

		if(!whole_line.empty() && !isComment) {
			auto tokens = splitIntoTokens(spaceDelimsOps(whole_line), " ");

			for(const auto current : tokens) {
				//If there is no comment, isComment can still be true
				if(hasComment(current, 0)) {
					isComment = true;
				}

				if(!isComment) {
					auto tt = Keywords::getTokenType(current);
					toRet.push_back(std::make_unique<Token>(tt, current, line));
				}
			}
		}
	}

	return toRet;
}

//Adds a space before and after operators
std::string Loader::spaceDelimsOps(const std::string& line) {
	using std::string;
	string toRet;

	//TODO: Convert to std::for_each
	for(unsigned i = 0; i < line.length(); i++) {
		string poss_str = string(1, line.at(i));

		if(Keywords::isDelineator(poss_str)) {
			toRet.append(" " + poss_str + " ");
		} else if(Keywords::isOperator(poss_str)) {
			toRet.append(" " + poss_str);

			string next_str = string(1, line.at(i + 1));
			if(Keywords::isOperator(next_str)) {
				toRet.append(next_str);
				i++;
			}
			toRet.append(" ");
		} else {
			toRet.append(poss_str);
		}
	}

	return toRet;
}

//Splits a string at each delimiter
std::vector<std::string> Loader::splitIntoTokens(const std::string& str, const std::string& delim) {
	std::vector<std::string> tokens;
	size_t prev = 0, pos = 0;

	//TODO: Support strings with spaces
	do {
		pos = str.find(delim, prev);
		if(pos == std::string::npos) pos = str.length();

		const size_t nextString = str.find('\"', prev);
		if(nextString < pos) {
			prev = nextString + 1;
			pos = str.find('\"', pos);
		}

		std::string token = str.substr(prev, pos - prev);
		if(!token.empty()) tokens.push_back(token);

		prev = pos + delim.length();
	} while(pos < str.length() && prev < str.length());
	return tokens;
}
