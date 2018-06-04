#ifndef _PARSER
#define _PARSER

#include "Keywords.h"
#include <vector>

struct Node {

	Token* token;
	Node* parent;
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

	inline void linkNodes (Node* parent, Node* child) { 
		if (child->parent == NULL) {
			child->parent = parent;
			parent->children.push_back (child);
		}
	}

public:
	void parseTokens (const std::vector<Token*>& tokenList);
};

#endif // !_PARSER


