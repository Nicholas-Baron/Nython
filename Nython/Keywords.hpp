#ifndef _KEYWORDS
#define _KEYWORDS

#include "VectorUtil.hpp"

#include <vector>
#include <string>

enum class TokenType { IDENTIFIER, OPERATOR, DELINEATOR, COMMAND, TYPE, LITERAL };
enum class VariableType { VOID, INT, STRING, FLOAT, CHAR, BOOL };

std::ostream& operator<<(std::ostream& lhs, const TokenType& rhs);
std::ostream& operator<<(std::ostream& lhs, const VariableType& rhs);

struct Token {
	TokenType type;
	std::string text;
	unsigned line_number;
	Token(TokenType type_in, const std::string& txt, unsigned line) : type(type_in), text(txt), line_number(line) {}
};

std::ostream& operator<<(std::ostream& lhs, const Token& rhs);
bool operator==(const Token& lhs, const Token& rhs) noexcept;

namespace Keywords {
	bool isDelineator(const std::string& item);
	bool isOperator(const std::string& item);
	bool isCommand(const std::string& item);

	inline bool isFirstOfPairDelin(const Token& tok) { return tok.type == TokenType::DELINEATOR && tok.text == "("; }
	inline bool isSecondOfPairDelin(const Token& tok) { return tok.type == TokenType::DELINEATOR && tok.text == ")"; }
	inline bool nonPairedDelin(const Token& tok) { return tok.type == TokenType::DELINEATOR && !isFirstOfPairDelin(tok) && !isSecondOfPairDelin(tok); }

	bool isBinaryOp(const Token& tok);
	bool isBoolOp(const Token* tok);

	inline bool isAssignment(const Token* tok) { return tok != nullptr && tok->type == TokenType::OPERATOR && isBinaryOp(*tok) && (tok->text == "=" || tok->text == "+="); }
	inline bool isAssignAfterOp(const Token* tok) { return isAssignment(tok) && tok->text.at(0) != '='; }
	inline bool isUnaryOp(const Token* tok) { return tok != nullptr && !isBinaryOp(*tok) && isOperator(tok->text); }
	inline bool canUseIncre(const VariableType& type) noexcept { return type == VariableType::FLOAT || type == VariableType::INT; }

	inline bool isLoopStart(const Token* tok) { return tok->type == TokenType::COMMAND && tok->text == "repeat"; }
	inline bool isConditionalStart(const Token* tok) { return tok->type == TokenType::COMMAND && (tok->text == "if" || tok->text == "elif"); }
	inline bool isEndOfLoop(const Token* tok) { return tok->type == TokenType::COMMAND && tok->text == "loop"; }
	inline bool isEndOfConditional(const Token& tok) { return tok.type == TokenType::COMMAND && (tok.text == "elif" || tok.text == "else" || tok.text == "endif"); }
	inline bool needsParameter(const Token* tok) {
		return tok->type == TokenType::COMMAND && (tok->text == "print" || tok->text == "if" || tok->text == "elif" || tok->text == "else");
	}

	bool getBoolFromToken(const Token& tok);

	inline bool isFuncEnd(const Token* tok) { return tok->type == TokenType::COMMAND && (tok->text == "ret" || tok->text == "return"); }
	//Returns the optimal type for a variable
	VariableType bestFit(const Token* tok);
	inline bool compatibleVarTypes(const VariableType origin, const VariableType dest) noexcept {
		if(origin == dest) {
			return true;
		} else if(origin == VariableType::VOID || dest == VariableType::VOID) {
			return false;
		} else if(origin == VariableType::INT && dest == VariableType::FLOAT) {
			return true;
		}

		return false;
	}

	TokenType getTokenType(const std::string& text);
	VariableType getVarType(const Token& tok);

	void printKeywords();

	template<class T, class U>
	inline bool opTest(const std::string& op, T left, U right) {
		if(op == "<") {
			return left < right;
		} else if(op == "==") {
			return left == right;
		} else if(op == ">") {
			return left > right;
		} else if(op == ">=") {
			return left >= right;
		} else if(op == "<=") {
			return left <= right;
		}

		std::cerr << "[ERR] Unimplemented test operator: " << op << std::endl;
		return false;
	}

	int opMath(const std::string& op, int left, int right);
	float opMath(const std::string& op, float left, float right);

	template<class T>
	inline void opUnary(const std::string& op, T& var) {
		if(op == "++") {
			var++;
		} else if(op == "--") {
			var--;
		} else if(op == "!") {
			var = !var;
		} else {
			std::cerr << "[ERR] Unimplemented unary operator: " << op << std::endl;
		}
	}
}

template<>
inline void Keywords::opUnary(const std::string & op, bool& var) {
	if(op == "!") {
		var = !var;
	} else {
		std::cerr << "[ERR] Unimplemented bool unary operator: " << op << std::endl;
	}
}
#endif // !_KEYWORDS
