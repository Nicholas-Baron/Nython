#include "Program.h"
#include <iostream>

unsigned Program::getFuncDef (const std::string& name) const {

	unsigned start = 0;
	for (unsigned i = 0; i < SYNTAX.size(); i++) {
		auto nod = SYNTAX[i];
		if(nod) {
			if(nod->token->text == name) {
				start = i;
			}
		}
	}

	return start;
}

void Program::allocVariable (Node * assign) { 

	std::string name = assign->children[0]->token->text;
	VariableType type = Keywords::getVarType(*(assign->children[0]->children[0]->token));
	void* value = &(assign->children[1]->token->text);

	variables[std::pair<VariableType, std::string>(type, name)] = value;

	std::cout << "Created variable " << name << std::endl;
}

bool Program::testVariable(Node * test) {
	
	//Make <, >, == work
}

void Program::removeVar (Token * id) { }


void Program::run (const std::string& func) {

	currentLine = getFuncDef (func) + 1;
	
	while (!finished) {
		auto line = SYNTAX[currentLine];
		auto tokType = line->token->type;
		auto tokTxt = line->token->text;

		if (tokType == TokenType::IDENTIFIER) {
			stack.push (currentLine);
			run (tokTxt);
		} else if(tokType == TokenType::COMMAND){
			if (tokTxt == "repeat") {
				loop (line);
			} else if (tokTxt == "ret" || tokTxt == "return"){
				exitFunction(line);
			}
		}

		currentLine++;
	}

}

void Program::loop (Node* line) { 

	if (line->children.size() == 4) {
		allocVariable (line->children[0]->children[0]);
		testVariable(line->children[1]->children[0]);
	}
	//FINISH LOOP
}

void Program::exitFunction(Node* line) {
	if(stack.empty()) {
		finished = true;
	} else {
		currentLine = stack.top();
		stack.pop();
	}

}
