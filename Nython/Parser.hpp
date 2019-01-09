#ifndef _PARSER
#define _PARSER

#include "Keywords.hpp"

struct Node {
	const std::shared_ptr<const Token> token;
	std::vector<std::shared_ptr<Node>> children;

	Node(const std::shared_ptr<const Token> tok_in) noexcept : token(tok_in) {}

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
	std::vector<std::shared_ptr<Node>> roots;
	std::vector<std::shared_ptr<const Token>> tokenList;

	void parseAfterDelineator(Node* delin, unsigned pos);
	void parseOperator(Node* addTo, unsigned& pos);
	void parseCommand(Node* comm, unsigned& pos, bool isLast);
	void parseSequence(Node* addTo, unsigned start, unsigned end, bool multiLine);
	void parseIdentifier(Node* id, unsigned& pos, const Token* next);
	std::shared_ptr<Node> parseToken(const std::shared_ptr<const Token>& t, unsigned& pos, bool isLast);
	std::vector<std::shared_ptr<Node>> parseTokens();
	void trimRoots();

	public:
	Parser() noexcept {}
	Parser(const std::vector<std::shared_ptr<const Token>>& tokenList_in);

	inline const auto& parsedTokens() const noexcept { return roots; }
	inline bool isReady() const noexcept { return ready; }
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
