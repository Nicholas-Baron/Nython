#include "ActionTree.h"

#include <iostream>
#include <iomanip>

Action* parseNode(Node* n, bool& finished);
Action* parseCall(Node* line, bool& finished);

Action* parseCommand(Node* line, bool& finished) {
	Action* current = new Action;
	current->tok = line->token;
	current->type = CALL;
	current->result = VariableType::VOID;
	if(Keywords::isConditionalStart(*(line->token))) {
		current->type = DECISION;
		current->children.push_back(parseNode(line->children[0]->children[0], finished));
	} else if(line->token->text == "else") {
		current->type = DECISION;
		current->children.push_back(parseNode(line->children[0], finished));
	} else if(line->token->text == "print"){
		current->result = VariableType::VOID;
		current->children.push_back(parseNode(line->children[0], finished));
	} else if(line->token->text == "repeat") {
		current->children.push_back(parseNode(line->children[0]->children[0], finished));
		current->children.push_back(parseNode(line->children[1]->children[0], finished));
		current->children.push_back(parseNode(line->children[2]->children[0], finished));
		for(unsigned i = 0; i < line->children[3]->children.size(); i++) {
			current->children.push_back(parseNode(line->children[3]->children[i], finished));
		}
	} else if(Keywords::isFuncEnd(*(line->token))) {
		if(line->children.size() > 0) {
			current->children.push_back(parseNode(line->children[0], finished));
			current->result = current->children[0]->result;
		}
		finished = true;
	} else if(line->token->text == "endline") { //Left for potential further edits
	} else if(Keywords::isEndOfLoop(*(line->token))) { //Left for potential further edits
	} 

	return current;
}

Action* parseDefinition(Node* line, bool& finished) {
	
	Action* current = new Action;
	current->result = Keywords::getVarType(*(line->children[0]->token));
	current->tok = line->token;
	current->type = DEFINITION;
	if(line->children.size() >= 2) {
		for(unsigned i = 1; i < line->children.size(); i++) {
			current->children.push_back(parseNode(line->children[i]->children[0], finished));
		}
	}
	return current;
}

Action* parseOperator(Node* line, bool& finished) { 
	Action* current = new Action; 
	current->tok = line->token;
	current->type = OPCALL;
	current->children.push_back(parseNode(line->children[0], finished));
	if(Keywords::isBinaryOp(*(line->token))) {
		current->children.push_back(parseNode(line->children[1], finished));
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

Action* parseCall(Node* line, bool& finished) {
	Action* current = new Action;
	current->tok = line->token;
	current->type = CALL;
	if(line->children.size() >= 1) {
		for(unsigned i = 0; i < line->children.size(); i++) {
			current->children.push_back(parseNode(line->children[i]->children[0], finished));
		}
	}
	return current;
}

Action* parseNode(Node* n, bool& finished){
	Action* toRet = NULL;
	const auto& lineType = n->token->type;
	if(lineType == TokenType::IDENTIFIER) {
		if(n->children.size() == 0 || n->children[0]->token->type != TokenType::TYPE) {
			toRet = parseCall(n, finished);
		} else {
			toRet = parseDefinition(n, finished);
		}
	} else if(lineType == TokenType::COMMAND) {
		toRet = parseCommand(n, finished);
	} else if(lineType == TokenType::OPERATOR) {
		toRet = parseOperator(n, finished);
	} else if(lineType == TokenType::LITERAL) {
		toRet = new Action;
		toRet->tok = n->token;
		toRet->type = LITVAL;
		toRet->result = Keywords::bestFit(*(n->token));
	}

	return toRet;
}

Action* parseActionTree(std::vector<Node*> lines, unsigned& pos) {
	
	bool finished = false;
	Action* toRet = parseNode(lines[pos], finished);
	while(!finished) {
		pos++;
		toRet->children.push_back(parseNode(lines[pos], finished));
	}
	return toRet;
}

void ActionTree::writeActionTree(std::vector<Node*> parsed) {
	
	for(unsigned i = 0; i < parsed.size(); i++) {
		actions.push_back(parseActionTree(parsed, i));
	}
}

void ActionTree::printActionTree(const Action* node, unsigned currentDepth) const {
	std::cout << std::setw(7)<< *(node->tok) << std::setw(14);
	switch(node->type) {
		case ActionType::CALL: std::cout << "CALL"; break;
		case ActionType::DEFINITION: std::cout << "DEFINITION"; break;
		case ActionType::LITVAL: std::cout << "LITERAL"; break;
		case ActionType::OPCALL: std::cout << "OPERATOR CALL"; break;
		case ActionType::VARIABLE: std::cout << "VARIABLE"; break;
		case ActionType::DECISION: std::cout << "DECISION"; break;
	}
	std::cout << std::setw(14) <<node->result << std::setw(7)<< currentDepth << std::endl;

	for(unsigned i = 0; i < node->children.size(); i++) {
		printActionTree(node->children[i], currentDepth + 1);
	}
}
