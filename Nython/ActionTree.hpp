#ifndef _ACTION_TREE
#define _ACTION_TREE

#include "Keywords.hpp"
#include "Parser.hpp"

#include <vector>
#include <memory>

enum ActionType { DEFINITION, CALL, OPCALL, LITVAL, VARIABLE, PARAM, DECISION, NO_OP };

struct Action {
	const std::shared_ptr<const Token> tok;
	ActionType type;
	VariableType resultType;
	std::vector<std::unique_ptr<Action>> children;

	Action(const std::shared_ptr<const Token> tok_in, 
		   ActionType type_in = ActionType::NO_OP, 
		   VariableType result = VariableType::VOID) noexcept
		: tok(tok_in), type(type_in), resultType(result) {}
	
	//TODO: Use this function
	inline bool hasChildren() const noexcept { return children.size() > 0; }
	
	unsigned memFootprint() const {
		auto toRet = sizeof(*this);
		for(const auto& item : children) {
			toRet += item->memFootprint();
		}
		return toRet;
	}

	//TODO: Define assign and move
};

struct Definition {
	VariableType type;
	bool isFunc;
	std::string name;
};

using DefinitonList = std::vector<Definition>;

class ActionTree {
private:
	std::vector<std::unique_ptr<Action>> actions;
	DefinitonList definedItems;

public:
	void writeActionTreeList(const Parser& parsed);
	
	inline const auto& actionList() const noexcept { return actions; }
	
	void printActionTree(const Action * const node, unsigned currentDepth = 0) const;
	
	unsigned memFootprint() const {
		auto toRet = sizeof(*this);
		for(const auto& item : actions) {
			toRet += item->memFootprint();
		}
		return toRet;
	}
};

#endif // !_ACTION_TREE
