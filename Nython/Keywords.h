#ifndef _KEYWORDS
#define _KEYWORDS

#include "VectorUtil.h"

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
};

std::ostream& operator<<(std::ostream& lhs, const Token& rhs);
bool operator==(const Token& lhs, const Token& rhs);

class Keywords {
private:
	static std::vector<std::string> operators, delineators, commands, types;

public:
	inline static bool isDelineator(const std::string& item) { return VectorUtil::contains(delineators, item); }
	inline static bool isOperator(const std::string& item) { return VectorUtil::contains(operators, item); }
	inline static bool isCommand(const std::string& item) { return VectorUtil::contains(commands, item); }

	inline static bool isFirstOfPairDelin(const Token* tok) { return tok->type == TokenType::DELINEATOR && tok->text == "("; }
	inline static bool isSecondOfPairDelin(const Token* tok) { return tok->type == TokenType::DELINEATOR && tok->text == ")"; }
	inline static bool nonPairedDelin(const Token* tok) { return tok->type == TokenType::DELINEATOR && !isFirstOfPairDelin(tok) && !isSecondOfPairDelin(tok); }

	inline static bool isBinaryOp(const Token* tok) { return isOperator(tok->text) && tok->text != "++" && tok->text != "--" && tok->text != "!"; }
	inline static bool isBoolOp(const Token* tok) {
		return isOperator(tok->text) && (tok->text[0] == '<' || tok->text[0] == '>' || tok->text[0] == '!' || tok->text == "==");
	}
	inline static bool isAssignment(const Token* tok) { return tok->type == TokenType::OPERATOR && isBinaryOp(tok) && (tok->text == "=" || tok->text == "+="); }
	inline static bool isAssignAfterOp(const Token* tok) { return isAssignment(tok) && tok->text[0] != '='; }
	inline static bool isUnaryOp(const Token* tok) { return !isBinaryOp(tok) && isOperator(tok->text); }
	inline static bool canUseIncre(const VariableType& type) { return type == VariableType::FLOAT || type == VariableType::INT; }

	inline static bool isLoopStart(const Token* tok) { return tok->type == TokenType::COMMAND && tok->text == "repeat"; }
	inline static bool isConditionalStart(const Token* tok) { return tok->type == TokenType::COMMAND && (tok->text == "if" || tok->text == "elif"); }
	inline static bool isEndOfLoop(const Token* tok) { return tok->type == TokenType::COMMAND && tok->text == "loop"; }
	inline static bool isEndOfConditional(const Token* tok) { return tok->type == TokenType::COMMAND && (tok->text == "elif" || tok->text == "else" || tok->text == "endif"); }
	inline static bool needsParameter(const Token* tok) {
		return tok->type == TokenType::COMMAND && (tok->text == "print" || tok->text == "if" || tok->text == "elif" || tok->text == "else");
	}

	static bool getBoolFromToken(const Token* tok);

	inline static bool isFuncEnd(const Token* tok) { return tok->type == TokenType::COMMAND && (tok->text == "ret" || tok->text == "return"); }
	//Returns the optimal type for a variable
	static VariableType bestFit(const Token& tok);
	inline static bool compatibleVarTypes(const VariableType origin, const VariableType dest) {
		if(origin == dest) {
			return true;
		} else if(origin == VariableType::VOID || dest == VariableType::VOID) {
			return false;
		} else if(origin == VariableType::INT && dest == VariableType::FLOAT) {
			return true;
		}

		return false;
	}

	static TokenType getTokenType(const std::string& text);
	static VariableType getVarType(const Token& tok);

	static void printKeywords();

	template<class T, class U>
	static bool opTest(const std::string& op, T left, U right);

	static int opMath(const std::string& op, int left, int right);
	static float opMath(const std::string& op, float left, float right);

	template<class T>
	static void opUnary(const std::string& op, T& var);
};

template<class T, class U>
inline bool Keywords::opTest(const std::string & op, T left, U right) {
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

inline int Keywords::opMath(const std::string & op, int left, int right) {
	if(op == "+") {
		return left + right;
	} else if(op == "-") {
		return left - right;
	} else if(op == "*") {
		return left * right;
	} else if(op == "/") {
		return left / right;
	} else if(op == "%") {
		return left % right;
	}

	std::cerr << "[ERR] Unimplemented int math operator: " << op << std::endl;
	return 0;
}

inline float Keywords::opMath(const std::string & op, float left, float right) {
	if(op == "+") {
		return left + right;
	} else if(op == "-") {
		return left - right;
	} else if(op == "*") {
		return left * right;
	} else if(op == "/") {
		return left / right;
	}

	std::cerr << "[ERR] Unimplemented float math operator: " << op << std::endl;
	return 0;
}

template<class T>
inline void Keywords::opUnary(const std::string & op, T& var) {
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

template<>
inline void Keywords::opUnary(const std::string & op, bool& var) {
	if(op == "!") {
		var = !var;
	} else {
		std::cerr << "[ERR] Unimplemented bool unary operator: " << op << std::endl;
	}
}

#endif // !_KEYWORDS
