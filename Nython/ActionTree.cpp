#include "ActionTree.h"

#include <iostream>
#include <iomanip>

Action* parseNode(Node* n, bool& finished, DefinitonList& definitions);
Action* parseCall(Node* line, bool& finished, DefinitonList& defs);

unsigned conditionalLvl = 0;

Action* parseCommand(Node* line, bool& finished, DefinitonList& defs) {
	Action* current = new Action;
	current->tok = line->token;
	current->type = CALL;
	current->resultType = VariableType::VOID;
	if(Keywords::isConditionalStart(line->token)) {
		current->type = DECISION;
		conditionalLvl++;
		current->children.push_back(parseNode(line->children[0]->children[0], finished, defs));
		current->children.push_back(parseNode(line->children[1]->children[0], finished, defs));
		conditionalLvl--;
	} else if(Keywords::isEndOfConditional(line->token) && line->token->text != "endif") {
		current->type = DECISION;
		current->children.push_back(parseNode(line->children[0], finished, defs));
	} else if(line->token->text == "print"){
		current->resultType = VariableType::VOID;
		current->children.push_back(parseNode(line->children[0], finished, defs));
	} else if(Keywords::isLoopStart(line->token)) {
		current->children.push_back(parseNode(line->children[0]->children[0], finished, defs));
		current->children.push_back(parseNode(line->children[1]->children[0], finished, defs));
		current->children.push_back(parseNode(line->children[2]->children[0], finished, defs));
		for(unsigned i = 0; i < line->children[3]->children.size(); i++) {
			current->children.push_back(parseNode(line->children[3]->children[i], finished, defs));
		}
	} else if(Keywords::isFuncEnd(line->token)) {
		if(line->children.size() > 0) {
			current->children.push_back(parseNode(line->children[0], finished, defs));
			current->resultType = current->children[0]->resultType;
		}
		if(conditionalLvl == 0) {
			finished = true;
		}
	} else if(line->token->text == "endline") { //Left for potential further edits
	} else if(Keywords::isEndOfLoop(line->token)) { //Left for potential further edits
	} 

	return current;
}

Action* parseDefinition(Node* line, bool& finished, DefinitonList& defs) {
	
	Action* current = new Action;
	current->resultType = Keywords::getVarType(*(line->children[0]->token));
	current->tok = line->token;
	current->type = DEFINITION;
	
	const auto isFunction = line->children.size() >= 2 || current->resultType == VariableType::VOID;
	if(isFunction) {
		for(unsigned i = 1; i < line->children.size(); i++) {
			current->children.push_back(parseNode(line->children[i]->children[0], finished, defs));
		}
	}
	
	defs.push_back(Definition{current->tok->text, current->resultType, isFunction});
	
	return current;
}

Action* parseOperator(Node* line, bool& finished, DefinitonList& defs) { 
	Action* current = new Action; 
	current->tok = line->token;
	current->type = OPCALL;
	current->children.push_back(parseNode(line->children[0], finished, defs));
	if(Keywords::isBinaryOp(line->token)) {
		current->children.push_back(parseNode(line->children[1], finished, defs));
	}

	if(Keywords::isBoolOp(line->token)) {
		current->resultType = VariableType::BOOL;
	} else if(Keywords::isAssignment(line->token)) {
		current->resultType = VariableType::VOID;
	} else {
		current->resultType = current->children[0]->resultType;
	}
	return current;
}

Action* parseCall(Node* line, bool& finished, DefinitonList& defs) {
	Action* current = new Action;
	current->tok = line->token;
	
	for(size_t i = 0; i < defs.size(); i++) {
		if(defs[i].name == current->tok->text) {
			current->resultType = defs[i].type;
			if(defs[i].isFunc) { 
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
		for(unsigned i = 0; i < line->children.size(); i++) {
			current->children.push_back(parseNode(line->children[i]->children[0], finished, defs));
		}
	}
	return current;
}

Action* parseNode(Node* n, bool& finished, DefinitonList& defs){
	Action* toRet = NULL;
	const auto& lineType = n->token->type;
	if(lineType == TokenType::IDENTIFIER) {
		if(n->children.size() == 0 || n->children[0]->token->type != TokenType::TYPE) {
			toRet = parseCall(n, finished, defs);
		} else {
			toRet = parseDefinition(n, finished, defs);
		}
	} else if(lineType == TokenType::COMMAND) {
		toRet = parseCommand(n, finished, defs);
	} else if(lineType == TokenType::OPERATOR) {
		toRet = parseOperator(n, finished, defs);
	} else if(lineType == TokenType::LITERAL) {
		toRet = new Action;
		toRet->tok = n->token;
		toRet->type = LITVAL;
		toRet->resultType = Keywords::bestFit(*(n->token));
	}

	return toRet;
}

Action* parseActionTree(std::vector<Node*> lines, unsigned& pos, DefinitonList& definitions) {
	
	bool finished = false;
	Action* toRet = parseNode(lines[pos], finished, definitions);
	while(!finished) {
		pos++;
		toRet->children.push_back(parseNode(lines[pos], finished, definitions));
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
		actions.push_back(parseActionTree(parsed, i, definedItems));
	}
}

void ActionTree::printActionTree(const Action* node, unsigned currentDepth) const {
	std::cout << std::setw(7)<< *(node->tok) << std::setw(14);
	switch(node->type) {
		case ActionType::DEFINITION: std::cout << "DEFINITION"; break;
		case ActionType::CALL: std::cout << "CALL"; break;
		case ActionType::OPCALL: std::cout << "OPERATOR CALL"; break;
		case ActionType::LITVAL: std::cout << "LITERAL"; break;
		case ActionType::VARIABLE: std::cout << "VARIABLE"; break;
		case ActionType::PARAM: std::cout << "PARAMETER"; break;
		case ActionType::DECISION: std::cout << "DECISION"; break;
	}
	std::cout << std::setw(14) <<node->resultType << std::setw(7)<< currentDepth << std::endl;

	for(unsigned i = 0; i < node->children.size(); i++) {
		printActionTree(node->children[i], currentDepth + 1);
	}
}
