#include "Loader.h"

#include <iostream>
#include <sstream>

std::vector<std::string> fileContents(const std::string& location) {

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

std::vector<Token> tokens(std::vector<std::string> program) {
	std::vector<Token> toRet;
	
	for(unsigned line = 0; line < program.size(); line++) {
		bool isComment = false;
		auto whole_line = trim(program[line]);
		whole_line = spaceDelimsOps(whole_line);

		if(hasComment(whole_line)) {
			const auto first_comment = whole_line.find_first_of("//");
			if(first_comment == 0) {
				isComment = true;
			}
		}
		
		if(!whole_line.empty() && !isComment) {
			auto tokens = split(whole_line, " ");
			
			for(unsigned i = 0; i < tokens.size(); i++) {
				
				auto current = tokens[i];

				if(hasComment(current)) {
					const auto first_comment = current.find_first_of("//");
					if(first_comment == 0) {
						isComment = true;
					}
				}

				if(!isComment) {
					TokenType tt = Keywords::getTokenType(current);

					toRet.push_back(Token{tt, current, line});
				}
			}
		}
	}

	return toRet;
}

std::string spaceDelimsOps(const std::string& line) {

	std::string first_pass;
	//1 char
	for(unsigned i = 0; i < line.length(); i++) {
		std::string poss_str = std::string(1, line[i]);

		if(Keywords::isDelineator(poss_str)) {
			first_pass.append(" " + poss_str + " ");
		} else if (Keywords::isOperator(poss_str)) {
			first_pass.append(" " + poss_str + " ");
		} else { 
			first_pass.append(poss_str); 
		}
	}

	std::string toRet;
	//2 char
	for(unsigned i = 0; i < first_pass.length(); i++) {
		std::string poss_str = first_pass.substr(i, 2);
		//Skip 1 char if you find a 2-char operator
		if(Keywords::isDelineator(poss_str)) {
			toRet.append(" " + poss_str + " ");
			i++;
		} else if(Keywords::isOperator(poss_str)) {
			toRet.append(" " + poss_str + " ");
			i++;
		} else {
			toRet.append(std::string(1, first_pass[i]));
		}
	}

	return toRet;
}

std::vector<std::string> split(const std::string& str, const std::string& delim) {
	std::vector<std::string> tokens;
	size_t prev = 0, pos = 0;
	do {
		pos = str.find(delim, prev);
		if(pos == std::string::npos) pos = str.length();
		std::string token = str.substr(prev, pos - prev);
		if(!token.empty()) tokens.push_back(token);
		prev = pos + delim.length();
	} while(pos < str.length() && prev < str.length());
	return tokens;
}