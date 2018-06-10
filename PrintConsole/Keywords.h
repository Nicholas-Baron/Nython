#ifndef _KEYWORDS
#define _KEYWORDS

#include <cctype>
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

	template<class T>
	inline static bool isContained (std::vector<T> set, T element) {
		bool contains = false;
		for(unsigned i = 0; i < set.size(); i++) {
			if(element == set[i]) contains = true;
		}
		return contains;
	}
public:
	inline static bool isDelineator(const std::string& item) { return isContained(delineators, item); }
	inline static bool isOperator(const std::string& item) { return isContained(operators, item); }
	inline static bool isFirstOfPairDelin(const Token& tok) { return tok.type == TokenType::DELINEATOR && tok.text == "("; }
	inline static bool isSecondOfPairDelin(const Token& tok) { return tok.type == TokenType::DELINEATOR && tok.text == ")"; }
	static bool isBinaryOp(const Token& tok);
	inline static bool isLoopStart(const Token& tok) { return tok.type == TokenType::COMMAND && tok.text == "repeat"; }
	inline static bool isSecondOfPairLoop(const Token& tok) { return tok.type == TokenType::COMMAND && tok.text == "loop"; }
	inline static bool needsParameter(const Token& tok) { return tok.type == TokenType::COMMAND && tok.text == "print"; }
	inline static bool isFuncEnd(const Token& tok) { return tok.type == TokenType::COMMAND && (tok.text == "ret" || tok.text == "return"); }

	static TokenType getTokenType(const std::string& text);
	static VariableType getVarType(const Token& tok);

	static void printKeywords();

	template<class T, class U>
	static bool opProcess (const std::string& op, T left, U right);
};

template<class T>
std::ostream& operator<<(std::ostream& lhs, std::vector<T> list) {
	for(unsigned i = 0; i < list.size(); i++) {
		lhs << list[i];
		if(i != list.size() - 1) {
			lhs << ", ";
		}
	}
	return lhs;
}

template<class T, class U>
inline bool Keywords::opProcess (const std::string & op, T left, U right) {
	if (op == "<") {
		return left < right;
	} else if (op == "==") {
		return left == right;
	} else if(op == ">"){
		return left > right;
	}
	return false;
}

#endif // !_KEYWORDS