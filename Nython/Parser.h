#ifndef _PARSER
#define _PARSER

#include "Keywords.h"

#define TOKEN_LIST const std::vector<Token*>& tokenList

struct Node {
	Token* token;
	std::vector<Node*> children;

	unsigned memFootprint() const {
		auto toRet = sizeof(*this);
		for(const auto& item : children) {
			toRet += item->memFootprint();
		}
		return toRet;
	}
};

class Parser {
private:
	bool ready = false;
	std::vector<Node*> roots;

	std::vector<Node*> parseTokens(TOKEN_LIST);
	void trimRoots();
public:
	Parser() {}
	Parser(TOKEN_LIST);

	inline std::vector<Node*> parsedTokens ( ) const { return roots; }
	inline bool isReady() const { return ready; }
	unsigned memFootprint() const {
		auto toRet = sizeof(*this);
		for(const auto& item : roots) {
			toRet += item->memFootprint();
		}
		return toRet;
	}

	static void readNode (Node* root, int depth = 0);
	
	/*inline static bool isDeclaration(Node* line) {
		return line->children.size()>=1 && line->children[0]->token->type == TokenType::TYPE;
	}*/
};

#endif // !_PARSER


