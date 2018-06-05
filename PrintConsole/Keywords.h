#ifndef _KEYWORDS
#define _KEYWORDS

#include <cctype>
#include <unordered_set>
#include <string>

enum class TokenType { IDENTIFIER, OPERATOR, DELINEATOR, COMMAND, TYPE, LITERAL };

struct Token {
	TokenType type;
	std::string token;
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
	inline static bool isFirstOfPairDelin(const Token& tok) { return tok.type == TokenType::DELINEATOR && tok.token == "("; }
	inline static bool isSecondOfPairDelin(const Token& tok) { return tok.type == TokenType::DELINEATOR && tok.token == ")"; }
	static bool isBinaryOp(const Token& tok);
	inline static bool isLoopStart(const Token& tok) { return tok.type == TokenType::COMMAND && tok.token == "repeat"; }
	inline static bool isSecondOfPairLoop(const Token& tok) { return tok.type == TokenType::COMMAND && tok.token == "loop"; }
	inline static bool needsParameter(const Token& tok) { return tok.type == TokenType::COMMAND && tok.token == "print"; }
	inline static bool isFuncEnd(const Token& tok) { return tok.type == TokenType::COMMAND && (tok.token == "ret" || tok.token == "return"); }

	static TokenType getTokenType(const std::string& text);

};

#endif // !_KEYWORDS
