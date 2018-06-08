#ifndef _KEYWORDS
#define _KEYWORDS

#include <cctype>
#include <unordered_set>
#include <string>

enum TokenType { IDENTIFIER, OPERATOR, DELINEATOR, COMMAND, TYPE, LITERAL };
enum VariableType {VOID, INT, STRING, FLOAT};

std::ostream& operator<<(std::ostream& lhs, const TokenType& rhs);

struct Token {
	TokenType type;
	std::string text;
	unsigned line_number;
};

std::ostream& operator<<(std::ostream& lhs, const Token& rhs);
bool operator==(const Token& lhs, const Token& rhs);

class Keywords {
private:
	static std::unordered_set<std::string> operators, delineators, commands, types;

	template<class T>
	inline static bool isContained (std::unordered_set<T> set, T element) {
		return set.find (element) != set.end ( );
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
};

#endif // !_KEYWORDS
