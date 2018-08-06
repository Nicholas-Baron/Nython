#ifndef _ACTION_TREE
#define _ACTION_TREE

#include "Keywords.h"
#include "Parser.h"

#include <vector>

enum ActionType { DEFINITION, CALL, OPCALL, LITVAL, VARIABLE, PARAM, DECISION };

struct Action {
	Token* tok;
	ActionType type;
	VariableType result;
	std::vector<Action*> children;
	inline bool hasChildren() const { return children.size() > 0; }
};

using DefinitonList = std::vector<std::pair<std::string, VariableType>>;

class ActionTree {
private:
	std::vector<Action*> actions;
	DefinitonList definedItems;
public:
	void writeActionTree(std::vector<Node*> parsed);
	inline std::vector<Action*> actionList() const { return actions; }
	void printActionTree(const Action* node, unsigned currentDepth = 0) const;
};

#endif // !_ACTION_TREE
