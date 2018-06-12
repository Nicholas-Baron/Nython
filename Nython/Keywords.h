#ifndef _KEYWORDS
#define _KEYWORDS

#include "VectorUtil.h"

#include <vector>
#include <string>

enum TokenType { IDENTIFIER, OPERATOR, DELINEATOR, COMMAND, TYPE, LITERAL };
enum VariableType {VOID, INT, STRING, FLOAT, CHAR};

std::ostream& operator<<(std::ostream& lhs, const TokenType& rhs);
std::ostream& operator<<(std::ostream& lhs, const VariableType& rhs);

struct Token {
	TokenType type;
	std::string text;
	unsigned line_number;
};

std::ostream& operator<<(std::ostream& lhs, const Token& rhs);
bool operator==(const Token& lhs, const Token& rhs);

class Keywords {
private:
	static std::vector<std::string> operators, delineators, commands, types;

public:
	inline static bool isDelineator(const std::string& item) { return contains(delineators, item); }
	inline static bool isOperator(const std::string& item) { return contains(operators, item); }
	inline static bool isFirstOfPairDelin(const Token& tok) { return tok.type == TokenType::DELINEATOR && tok.text == "("; }
	inline static bool isSecondOfPairDelin(const Token& tok) { return tok.type == TokenType::DELINEATOR && tok.text == ")"; }
	static bool isBinaryOp(const Token& tok);
	inline static bool isLoopStart(const Token& tok) { return tok.type == TokenType::COMMAND && tok.text == "repeat"; }
	inline static bool isConditionalStart(const Token& tok) { return tok.type == TokenType::COMMAND && (tok.text == "if" || tok.text == "elif"); }
	inline static bool isSecondOfPairLoop(const Token& tok) { return tok.type == TokenType::COMMAND && tok.text == "loop"; }
	inline static bool isEndOfConditional(const Token& tok) { return tok.type == TokenType::COMMAND && (tok.text == "elif" || tok.text == "else"); }
	inline static bool needsParameter(const Token& tok) { 
		return tok.type == TokenType::COMMAND && (tok.text == "print" || tok.text == "if" || tok.text == "elif"); 
	}
	inline static bool isAssignment(const Token& tok) { return tok.type == TokenType::OPERATOR && isBinaryOp(tok) && tok.text == "="; }
	inline static bool isFuncEnd(const Token& tok) { return tok.type == TokenType::COMMAND && (tok.text == "ret" || tok.text == "return"); }

	static TokenType getTokenType(const std::string& text);
	static VariableType getVarType(const Token& tok);

	static void printKeywords();

	template<class T, class U>
	static bool opProcess (const std::string& op, T left, U right);
};

template<class T, class U>
inline bool Keywords::opProcess (const std::string & op, T left, U right) {
	if (op == "<") {
		return left < right;
	} else if (op == "==") {
		return left == right;
	} else if(op == ">"){
		return left > right;
	} else if(op == ">=") {
		return left >= right;
	} else if(op == "<=") {
		return left <= right;
	}
	return false;
}

#endif // !_KEYWORDS