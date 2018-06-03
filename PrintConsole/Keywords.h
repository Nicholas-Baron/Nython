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

class Keywords {
private:
	static std::unordered_set<std::string> operators, delineators, commands, types;
public:
	template<class T>
	inline static bool isContained(std::unordered_set<T> set, T element) {
		return set.find(element) != set.end();
	}

	inline static bool isDelineator(const std::string& item) { return isContained(delineators, item); }
	inline static bool isOperator(const std::string& item) { return isContained(operators, item); }

	static TokenType getTokenType(const std::string& text);

};

#endif // !_KEYWORDS
