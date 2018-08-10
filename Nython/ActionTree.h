#ifndef _ACTION_TREE
#define _ACTION_TREE

#include "Keywords.h"
#include "Parser.h"

#include <vector>

enum ActionType { DEFINITION, CALL, OPCALL, LITVAL, VARIABLE, PARAM, DECISION };

struct Action {
	Token* tok;
	ActionType type;
	VariableType resultType;
	std::vector<Action*> children;
	inline bool hasChildren() const { return children.size() > 0; }
};

struct Definition {
	std::string name;
	VariableType type;
	bool isFunc;
};

using DefinitonList = std::vector<Definition>;

class ActionTree {
private:
	std::vector<Action*> actions;
	DefinitonList definedItems;
public:
	void writeActionTreeList(const Parser& parsed);
	inline std::vector<Action*> actionList() const { return actions; }
	void printActionTree(const Action* node, unsigned currentDepth = 0) const;
};

#endif // !_ACTION_TREE
