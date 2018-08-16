#ifndef _PARSER
#define _PARSER

#include "Keywords.h"

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
	std::vector<Token*> tokenList;

	void parseAfterDelineator(Node* delin, unsigned pos);
	void parseOperator(Node* addTo, unsigned& pos);
	void parseCommand(Node* comm, unsigned& pos, Token* next);
	void parseSequence(Node* addTo, unsigned& start, unsigned end, bool multiLine);
	void parseIdentifier(Node* id, unsigned& pos, Token* next);
	Node* parseToken(Token* t, unsigned& pos, Token* next, bool isLast);
	std::vector<Node*> parseTokens();
	void trimRoots();

public:
	Parser() {}
	Parser(const std::vector<Token*>& tokenList);

	inline std::vector<Node*> parsedTokens() const { return roots; }
	inline bool isReady() const { return ready; }
	unsigned memFootprint() const {
		auto toRet = sizeof(*this);
		for(const auto& item : roots) {
			toRet += item->memFootprint();
		}
		return toRet;
	}

	static void readNode(Node* root, int depth = 0);

	/*inline static bool isDeclaration(Node* line) {
		return line->children.size()>=1 && line->children[0]->token->type == TokenType::TYPE;
	}*/
};

#endif // !_PARSER
