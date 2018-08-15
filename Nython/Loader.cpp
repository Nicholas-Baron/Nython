#include "Loader.h"

#include <iostream>
#include <sstream>

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
std::vector<Token*> Loader::tokens(const std::vector<std::string>& program) {
	std::vector<Token*> toRet;

	for(unsigned line = 0; line < program.size(); line++) {
		bool isComment = false;
		auto whole_line = trim(program[line]);
		unsigned commentLoc = 0;

		if(hasComment(whole_line, commentLoc)) {
			std::cout << "Comment found on line #" << line << ", character " << commentLoc << std::endl;
		}

		if(!whole_line.empty() && !isComment) {
			auto tokens = splitIntoTokens(spaceDelimsOps(whole_line), " ");

			for(const auto current : tokens) {
				unsigned possibleComment = 0;
				if(hasComment(current, possibleComment)) {
					if(possibleComment == 0) {
						isComment = true;
					}
				}

				if(!isComment) {
					TokenType tt = Keywords::getTokenType(current);
					Token* tok = new Token{tt, current, line};
					toRet.push_back(tok);
				}
			}
		}
	}

	return toRet;
}

//Adds a space before and after operators
std::string Loader::spaceDelimsOps(const std::string& line) {
	std::string toRet;

	for(unsigned i = 0; i < line.length(); i++) {
		std::string poss_str = std::string(1, line[i]);
		std::string next_str = std::string(1, line[i + 1]);

		if(Keywords::isDelineator(poss_str)) {
			toRet.append(" " + poss_str + " ");
		} else if(Keywords::isOperator(poss_str)) {
			toRet.append(" " + poss_str);
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

		auto nextString = str.find('\"', prev);
		if(nextString < pos) {
			prev = nextString;
			pos = str.find('\"', pos) + 1;
		}

		std::string token = str.substr(prev, pos - prev);
		if(!token.empty()) tokens.push_back(token);

		prev = pos + delim.length();
	} while(pos < str.length() && prev < str.length());
	return tokens;
}
