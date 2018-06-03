#include "Keywords.h"

std::unordered_set<std::string>	Keywords::operators = {"=","<","++","->"};
std::unordered_set<std::string>	Keywords::delineators = {"(",")","|"};
std::unordered_set<std::string>	Keywords::commands = {"print","repeat","loop","ret","return"};
std::unordered_set<std::string>	Keywords::types = {"int","void"};

TokenType Keywords::getTokenType(const std::string & text) {

	const auto first_space = text.find_first_of(" ");
	if(first_space != std::string::npos) {
		throw "Can not get a token with a space in it";
	}

	TokenType type = TokenType::IDENTIFIER;

	if(isContained(operators, text)) {
		type = TokenType::OPERATOR;
	} else if(isContained(delineators, text)) {
		type = TokenType::DELINEATOR;
	} else if(isContained(commands, text)) {
		type = TokenType::COMMAND;
	} else if(isContained(types, text)) {
		type = TokenType::TYPE;
	} else if(text.find_first_of("\"") == 0 || isdigit(text[0])) {
		type = TokenType::LITERAL;
	}

	return type;
}