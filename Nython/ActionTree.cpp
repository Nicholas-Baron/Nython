#include "ActionTree.h"

#include <iostream>
#include <iomanip>

Action* parseNode(Node* n, bool& finished, DefinitonList& definitions);
Action* parseCall(Node* line, bool& finished, DefinitonList& defs);

Action* parseCommand(Node* line, bool& finished, DefinitonList& defs) {
	Action* current = new Action;
	current->tok = line->token;
	current->type = CALL;
	current->result = VariableType::VOID;
	if(Keywords::isConditionalStart(*(line->token))) {
		current->type = DECISION;
		current->children.push_back(parseNode(line->children[0]->children[0], finished, defs));
	} else if(line->token->text == "else") {
		current->type = DECISION;
		current->children.push_back(parseNode(line->children[0], finished, defs));
	} else if(line->token->text == "print"){
		current->result = VariableType::VOID;
		current->children.push_back(parseNode(line->children[0], finished, defs));
	} else if(line->token->text == "repeat") {
		current->children.push_back(parseNode(line->children[0]->children[0], finished, defs));
		current->children.push_back(parseNode(line->children[1]->children[0], finished, defs));
		current->children.push_back(parseNode(line->children[2]->children[0], finished, defs));
		for(unsigned i = 0; i < line->children[3]->children.size(); i++) {
			current->children.push_back(parseNode(line->children[3]->children[i], finished, defs));
		}
	} else if(Keywords::isFuncEnd(*(line->token))) {
		if(line->children.size() > 0) {
			current->children.push_back(parseNode(line->children[0], finished, defs));
			current->result = current->children[0]->result;
		}
		finished = true;
	} else if(line->token->text == "endline") { //Left for potential further edits
	} else if(Keywords::isEndOfLoop(*(line->token))) { //Left for potential further edits
	} 

	return current;
}

Action* parseDefinition(Node* line, bool& finished, DefinitonList& defs) {
	
	Action* current = new Action;
	current->result = Keywords::getVarType(*(line->children[0]->token));
	current->tok = line->token;
	current->type = DEFINITION;
	if(line->children.size() >= 2) {
		for(unsigned i = 1; i < line->children.size(); i++) {
			current->children.push_back(parseNode(line->children[i]->children[0], finished, defs));
		}
	}
	defs.push_back(std::make_pair(current->tok->text, current->result));

	return current;
}

Action* parseOperator(Node* line, bool& finished, DefinitonList& defs) { 
	Action* current = new Action; 
	current->tok = line->token;
	current->type = OPCALL;
	current->children.push_back(parseNode(line->children[0], finished, defs));
	if(Keywords::isBinaryOp(*(line->token))) {
		current->children.push_back(parseNode(line->children[1], finished, defs));
	}

	if(Keywords::isBoolOp(*(line->token))) {
		current->result = VariableType::BOOL;
	} else if(Keywords::isAssignment(*(line->token))) {
		current->result = VariableType::VOID;
	} else {
		current->result = current->children[0]->result;
	}
	return current;
}

Action* parseCall(Node* line, bool& finished, DefinitonList& defs) {
	Action* current = new Action;
	current->tok = line->token;
	
	for(size_t i = 0; i < defs.size(); i++) {
		if(defs[i].first == current->tok->text) {
			current->type = VARIABLE;
			current->result = defs[i].second;
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
		toRet->result = Keywords::bestFit(*(n->token));
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

void ActionTree::writeActionTree(std::vector<Node*> parsed) {
	
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
	std::cout << std::setw(14) <<node->result << std::setw(7)<< currentDepth << std::endl;

	for(unsigned i = 0; i < node->children.size(); i++) {
		printActionTree(node->children[i], currentDepth + 1);
	}
}
