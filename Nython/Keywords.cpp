#include "Keywords.h"

#include <algorithm>
#include <cctype>

std::vector<std::string>	Keywords::operators = {"=", "==", "<", "<=", ">", ">=", "++", "--", "->", "-", "+", "+=", "*", "**", "/", "%"};
std::vector<std::string>	Keywords::delineators = {"(",")","|"};
std::vector<std::string>	Keywords::commands = {"print","repeat","loop","ret","return", "endline", "if", "else", "elif", "endif"};
std::vector<std::string>	Keywords::types = {"int","void","float","string","char", "bool"};

bool Keywords::getBoolFromToken(const Token* value) {
	std::string lowered(value->text);
	std::transform(value->text.begin(), value->text.end(), lowered.begin(), std::tolower);

	if(lowered == "true" || lowered == "1") {
		return true;
	} else if(lowered == "false" || lowered == "0") {
		return false;
	}
	std::cout << "The value of " << value->text << " is recognized as true." << std::endl;
	return true;
}

VariableType Keywords::bestFit(const Token & tok) {
	
	if(tok.text[0] == '"') {
		return VariableType::STRING;
	}else if(tok.text == "true" || tok.text == "false") {
		return VariableType::BOOL;
	}else if(tok.text.length() == 3 && tok.text[0] == tok.text[2]) {
		return VariableType::CHAR;
	}
	
	bool canBeInt = true, canBeFloat = true, previousDot = false;
	for(unsigned i = 0; i < tok.text.length(); i++) {
		if(!isdigit(tok.text[i])) {
			canBeInt = false;
			if(tok.text[i] == '.') {
				if(previousDot) {
					canBeFloat = false;
				}
				previousDot = true;
			}
		}
	}

	if(canBeInt) {
		return VariableType::INT;
	} else if(canBeFloat) {
		return VariableType::FLOAT;
	}

	return VariableType::STRING;
}

TokenType Keywords::getTokenType(const std::string & text) {

	const auto first_space = text.find_first_of(" ");
	if(first_space != std::string::npos && text.find_first_of('"') != 0) {
		throw "Can not get a token with a space in it";
	}

	TokenType type = TokenType::IDENTIFIER;

	if(VectorUtil::contains(operators, text)) {
		type = TokenType::OPERATOR;
	} else if(VectorUtil::contains(delineators, text)) {
		type = TokenType::DELINEATOR;
	} else if(VectorUtil::contains(commands, text)) {
		type = TokenType::COMMAND;
	} else if(VectorUtil::contains(types, text)) {
		type = TokenType::TYPE;
	} else if(text.find_first_of('\"') == 0  || text.find_first_of('\'') == 0 || isdigit(text[0])) {
		type = TokenType::LITERAL;
	}

	return type;
}

VariableType Keywords::getVarType(const Token & tok) {
	
	if(tok.type != TokenType::TYPE) {
		return VariableType::VOID;
	}else if(tok.text == "int") {
		return VariableType::INT;
	}else if(tok.text == "string") {
		return VariableType::STRING;
	} else if(tok.text == "float") {
		return VariableType::FLOAT;
	} else if(tok.text == "char") {
		return VariableType::CHAR;
	} else {
		return VariableType::VOID;
	}
}

void Keywords::printKeywords() {
	
	std::cout << "Operators: " << operators << std::endl;
	std::cout << "Delineators: " << delineators << std::endl;
	std::cout << "Commands: " << commands << std::endl;
	std::cout << "Variable Types: " << types << std::endl;
}

std::ostream & operator<<(std::ostream & lhs, const TokenType & rhs) {
	switch(rhs) {
		case TokenType::COMMAND: lhs << "COMMAND"; break;
		case TokenType::DELINEATOR: lhs << "DELINEATOR"; break;
		case TokenType::IDENTIFIER: lhs << "IDENTIFIER"; break;
		case TokenType::LITERAL: lhs << "LITERAL"; break;
		case TokenType::OPERATOR: lhs << "OPERATOR"; break;
		case TokenType::TYPE: lhs << "TYPE"; break;
		default: lhs << "UNDEFINED";
	}
	return lhs;
}

std::ostream & operator<<(std::ostream & lhs, const VariableType & rhs) {
	switch (rhs) {
		case VariableType::CHAR: lhs << "CHAR"; break;
		case VariableType::FLOAT: lhs << "FLOAT"; break;
		case VariableType::INT: lhs << "INT"; break;
		case VariableType::STRING: lhs << "STRING"; break;
		case VariableType::VOID: lhs << "VOID"; break;
		case VariableType::BOOL: lhs << "BOOL"; break;
		default: lhs << "UNDEFINED";
	}
	return lhs;
}

std::ostream & operator<<(std::ostream & lhs, const Token& rhs) {
	lhs << rhs.text;
	return lhs;
}

bool operator==(const Token & lhs, const Token & rhs) {
	return lhs.line_number == rhs.line_number && lhs.type == rhs.type && lhs.text == rhs.text;
}
