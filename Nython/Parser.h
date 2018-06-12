#ifndef _PARSER
#define _PARSER

#include "Keywords.h"

#define TOKEN_LIST const std::vector<Token*>& tokenList

struct Node {
	Token* token;
	std::vector<Node*> children;
};

class Parser {
private:
	std::vector<Node*> roots;

	std::vector<Node*> parseTokens(TOKEN_LIST);
	void trimRoots();
public:
	Parser(TOKEN_LIST);
	inline std::vector<Node*> parsedTokens ( ) const { return roots; }
	static void readNode (Node* root, int depth = 0);
	inline static bool isDeclaration(Node* line) {
		return line->children.size()>=1 && line->children[0]->token->type == TokenType::TYPE;
	}
};

#endif // !_PARSER


