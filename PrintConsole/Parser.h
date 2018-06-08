#ifndef _PARSER
#define _PARSER

#include "Keywords.h"
#include <vector>
#include <unordered_map>

#define TOKEN_LIST const std::vector<Token*>& tokenList

struct Node {
	Token* token;
	std::vector<Node*> children;
};

class Parser {
private:
	static std::vector<Node*> roots;

	template<class T>
	inline static bool contains (const std::vector<T>& list, T find) { 
		return position (list, find) != -1;
	}

	template<class T>
	inline static int position (const std::vector<T>& list, T find) {
		int pos = -1;
		for (unsigned i = 0; i < list.size(); i++) {
			if (list[i] == find) {
				pos = i;
			}
		}
		return pos;
	}

	static void trimRoots();
public:
	static void parseTokens (TOKEN_LIST);
	inline static std::vector<Node*> parsedTokens ( ) { return roots; }
	static void readNode (Node* root, int depth = 0);
	inline static bool isDeclaration(Node* line) {
		return line->children.size()>=1 && line->children[0]->token->type == TokenType::TYPE;
	}
};

#endif // !_PARSER


