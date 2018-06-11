#include "Keywords.h"

#include <cctype>

std::vector<std::string>	Keywords::operators = {"=","<","++","->"};
std::vector<std::string>	Keywords::delineators = {"(",")","|"};
std::vector<std::string>	Keywords::commands = {"print","repeat","loop","ret","return", "endline", "if", "else", "elif"};
std::vector<std::string>	Keywords::types = {"int","void","float","string","char"};

bool Keywords::isBinaryOp(const Token & tok) {
	auto txt = tok.text;
	return (txt == "=") || (txt == "<") || (txt == "->");
}

TokenType Keywords::getTokenType(const std::string & text) {

	const auto first_space = text.find_first_of(" ");
	if(first_space != std::string::npos) {
		throw "Can not get a token with a space in it";
	}

	TokenType type = TokenType::IDENTIFIER;

	if(contains(operators, text)) {
		type = TokenType::OPERATOR;
	} else if(contains(delineators, text)) {
		type = TokenType::DELINEATOR;
	} else if(contains(commands, text)) {
		type = TokenType::COMMAND;
	} else if(contains(types, text)) {
		type = TokenType::TYPE;
	} else if(text.find_first_of("\"") == 0 || isdigit(text[0])) {
		type = TokenType::LITERAL;
	}

	return type;
}

VariableType Keywords::getVarType(const Token & tok) {
	
	if(tok.type != TokenType::TYPE) {
		return VOID;
	}else if(tok.text == "int") {
		return INT;
	}else if(tok.text == "string") {
		return STRING;
	} else if(tok.text == "float") {
		return FLOAT;
	} else {
		return VOID;
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
		case VariableType::FLOAT:lhs << "FLOAT"; break;
		case VariableType::INT:lhs << "INT"; break;
		case VariableType::STRING:lhs << "STRING"; break;
		case VariableType::VOID:lhs << "VOID"; break;
		default:lhs << "UNDEFINED";
	}
	return lhs;
}

std::ostream & operator<<(std::ostream & lhs, const Token& rhs) {
	lhs << rhs.text;
	return lhs;
}

bool operator==(const Token & lhs, const Token & rhs) {
	return lhs.line_number == rhs.line_number && lhs.type == rhs.type&&lhs.text == rhs.text;
}
