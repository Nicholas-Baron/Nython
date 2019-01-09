#include "ActionTree.hpp"

#include <iostream>
#include <iomanip>
#include <memory>

//Moving these two to the class requires moving all functions in this file into the class as well
std::unique_ptr<Action> parseNode(const Node * const n, bool& finished, DefinitonList& definitions);
auto parseCall(const Node * const line, bool& finished, DefinitonList& defs);

auto parseCommand(const Node * const line, bool& finished, DefinitonList& defs) {
	auto current = std::make_unique<Action>(line->token, CALL, VariableType::VOID);

	static unsigned conditionalLvl = 0;

	//TODO: Assert array locations
	if(Keywords::isConditionalStart(line->token.get())) {
		current->type = DECISION;
		conditionalLvl++;
		current->children.push_back(parseNode(line->children.front()->children.front().get(), finished, defs));
		current->children.push_back(parseNode(line->children.at(1)->children.front().get(), finished, defs));
		conditionalLvl--;
	} else if(Keywords::isEndOfConditional(*line->token) && line->token->text != "endif") {
		current->type = DECISION;
		current->children.push_back(parseNode(line->children.front().get(), finished, defs));
	} else if(line->token->text == "print") {
		current->resultType = VariableType::VOID;
		current->children.push_back(parseNode(line->children.front().get(), finished, defs));
	} else if(Keywords::isLoopStart(line->token.get())) {
		current->children.push_back(parseNode(line->children.front()->children.front().get(), finished, defs));
		current->children.push_back(parseNode(line->children.at(1)->children.front().get(), finished, defs));
		current->children.push_back(parseNode(line->children.at(2)->children.front().get(), finished, defs));

		//TODO: convert to std::for_each
		for(unsigned i = 0; i < line->children.at(3)->children.size(); i++) {
			current->children.push_back(parseNode(line->children.at(3)->children.at(i).get(), finished, defs));
		}
	} else if(Keywords::isFuncEnd(line->token.get())) {
		if(line->children.size() > 0) {
			current->children.push_back(parseNode(line->children.front().get(), finished, defs));
			current->resultType = current->children.front()->resultType;
		}
		if(conditionalLvl == 0) {
			finished = true;
		}
	} else if(line->token->text == "endline") { //Left for potential further edits
	} else if(Keywords::isEndOfLoop(line->token.get())) { //Left for potential further edits
	}

	return current;
}

auto parseDefinition(const Node * const line, bool& finished, DefinitonList& defs) {
	auto current = std::make_unique<Action>(line->token, DEFINITION, Keywords::getVarType(*(line->children.front()->token)));

	const bool isFunction = line->children.size() >= 2 || current->resultType == VariableType::VOID;
	if(isFunction) {
		for(unsigned i = 1; i < line->children.size(); i++) {
			current->children.push_back(parseNode(line->children.at(i)->children.front().get(), finished, defs));
		}
	}

	defs.push_back(Definition{current->resultType, isFunction, current->tok->text});

	return current;
}

auto parseOperator(const Node * const line, bool& finished, DefinitonList& defs) {
	auto current = std::make_unique<Action>(line->token, OPCALL);
	current->children.push_back(parseNode(line->children.front().get(), finished, defs));
	if(Keywords::isBinaryOp(*(line->token))) {
		current->children.push_back(parseNode(line->children.at(1).get(), finished, defs));
	}

	if(Keywords::isBoolOp(line->token.get())) {
		current->resultType = VariableType::BOOL;
	} else if(Keywords::isAssignment(line->token.get())) {
		current->resultType = VariableType::VOID;
	} else {
		current->resultType = current->children.front()->resultType;
	}
	return current;
}

auto parseCall(const Node * const line, bool& finished, DefinitonList& defs) {
	auto current = std::make_unique<Action>(line->token);

	for(const auto& def : defs) {
		if(def.name == current->tok->text) {
			current->resultType = def.type;
			if(def.isFunc) {
				current->type = CALL;
			} else {
				current->type = VARIABLE;
			}
		}
	}
	if(current->type != VARIABLE) {
		current->type = CALL;
	}

	if(line->children.size() >= 1) {
		for(const auto& child : line->children) {
			current->children.push_back(parseNode(child->children.front().get(), finished, defs));
		}
	}
	return current;
}

std::unique_ptr<Action> parseNode(const Node * const n, bool& finished, DefinitonList& defs) {
	const auto& lineType = n->token->type;

	switch(lineType) {
		case TokenType::IDENTIFIER:
			if(n->children.size() == 0 || n->children.front()->token->type != TokenType::TYPE) {
				return parseCall(n, finished, defs);
			} else {
				return parseDefinition(n, finished, defs);
			}
		case TokenType::COMMAND:
			return parseCommand(n, finished, defs);
		case TokenType::OPERATOR:
			return parseOperator(n, finished, defs);
		case TokenType::LITERAL:
			return std::make_unique<Action>(n->token, LITVAL, Keywords::bestFit(n->token.get()));
		case TokenType::DELINEATOR:
		case TokenType::TYPE:
		default:
			throw std::invalid_argument(n->token->text + " cannot parse this into a Node!");
	}
}

auto parseActionTree(const std::vector<std::shared_ptr<Node>>& lines, unsigned& pos, DefinitonList& definitions) {
	bool finished = false;
	auto toRet = parseNode(lines.at(pos).get(), finished, definitions);
	while(!finished) {
		pos++;
		toRet->children.push_back(std::move(parseNode(lines.at(pos).get(), finished, definitions)));
	}
	return toRet;
}

void ActionTree::writeActionTreeList(const Parser& parser) {
	if(!parser.isReady()) {
		std::cerr << "Parser was not ready!" << std::endl;
		throw "Unready Parser";
	}

	const auto& parsed = parser.parsedTokens();

	for(unsigned i = 0; i < parsed.size(); i++) {
		actions.push_back(std::move(parseActionTree(parsed, i, definedItems)));
	}
}

void ActionTree::printActionTree(const Action * const node, unsigned currentDepth) const {
	std::cout << std::setw(7) << *(node->tok) << std::setw(14);
	switch(node->type) {
		case ActionType::DEFINITION: std::cout << "DEFINITION"; break;
		case ActionType::CALL: std::cout << "CALL"; break;
		case ActionType::OPCALL: std::cout << "OPERATOR CALL"; break;
		case ActionType::LITVAL: std::cout << "LITERAL"; break;
		case ActionType::VARIABLE: std::cout << "VARIABLE"; break;
		case ActionType::PARAM: std::cout << "PARAMETER"; break;
		case ActionType::DECISION: std::cout << "DECISION"; break;
		case ActionType::NO_OP: std::cout << "NO OP"; break;
	}
	std::cout << std::setw(14) << node->resultType << std::setw(7) << currentDepth << std::endl;

	for(const auto& child : node->children) {
		printActionTree(child.get(), currentDepth + 1);
	}
}
