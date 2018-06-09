#include "Program.h"
#include <iostream>

unsigned Program::getFuncDef (const std::string& name) const {

	unsigned start = 0;
	for (unsigned i = 0; i < SYNTAX.size(); i++) {
		auto nod = SYNTAX[i];
		if(nod) {
			if(nod->token->text == name&&Parser::isDeclaration(nod)) {
				start = i;
			}
		}
	}

	return start;
}

void Program::allocVariable (Node * assign) { 

	std::string name = assign->children[0]->token->text;
	VariableType type = Keywords::getVarType(*(assign->children[0]->children[0]->token));
	varTyping[name] = type;
	
	std::string value = assign->children[1]->token->text;
	switch (type) {
		case INT:
			varsInt[name] = atoi (value.c_str ( ));
			break;
		case STRING:
			varsText[name] = value;
			break;
		case FLOAT:
			varsFloat[name] = atof (value.c_str ( ));
			break;
		case CHAR:
			varsChar[name] = value[1];
		case VOID:
		default:
			break;
	}

	std::cout << "Created variable " << name << std::endl;
}

void * Program::getValue(const std::string & name, const VariableType& expectedType) {
	auto type = varTyping[name];
	if(expectedType != type) {
		std::cout << "Unsupported action!" << std::endl;
		return nullptr;
	}
	switch (expectedType) {
		case INT:
			return &varsInt[name];
		case FLOAT:
			return &varsFloat[name];
		case CHAR:
			return &varsChar[name];
		case STRING:
			return &varsText[name];
		case VOID:
		default:
			return NULL;
	}
}

bool Program::testVariable(Node * test) {
	auto txt = test->token->text;
	auto left = test->children[0];
	auto right = test->children[1];
	
	VariableType typecast = VariableType::VOID;
	if(left->token->type == TokenType::IDENTIFIER) {
		typecast = varTyping[left->token->text];
	}
	if(right->token->type == TokenType::IDENTIFIER && typecast == VOID) {
		typecast = varTyping[left->token->text];
	}




	return false;
}

void Program::removeVar (Token * id) { 
	auto name = id->text;
	std::cout << "Deleting variable " << name << std::endl;
	auto type = varTyping[name];
	switch (type) {
		case INT:
			varsInt.erase (name); break;
		case FLOAT:
			varsFloat.erase (name); break;
		case STRING:
			varsText.erase (name); break;
		case CHAR:
			varsChar.erase (name); break;
	}
}


void Program::run (const std::string& func) {

	currentLine = getFuncDef (func) + 1;
	
	while (!finished) {
		auto line = SYNTAX[currentLine];
		auto tokType = line->token->type;
		auto tokTxt = line->token->text;

		Parser::readNode(line);
		std::cout << "  " << tokType<<" @ "<<currentLine;
		std::cout << std::endl;
		if (tokType == TokenType::IDENTIFIER) {
			stack.push (currentLine);
			run (tokTxt);
		} else if(tokType == TokenType::COMMAND){
			if (tokTxt == "repeat") {
				loop (line);
			} else if (tokTxt == "ret" || tokTxt == "return"){
				exitFunction(line);
			} else if(tokTxt == "print") {
				print(line);
			}
		}

		currentLine++;
	}

}

void Program::loop (Node* line) { 

	auto testNode = (line->children.size() == 4) ? line->children[1]->children[0] : line->children[0]->children[0];

	if (line->children.size() == 4) {
		auto initializer = line->children[0]->children[0];
		allocVariable (initializer);
		while(testVariable(testNode)) {
		
		}
		removeVar (initializer->children[0]->token);
	}
	//FINISH LOOP
}

void Program::print(Node * line) {
	auto tok = line->children[0]->token;

	if(tok->type==TokenType::LITERAL) {
		std::cout << tok->text;
	}
}

void Program::exitFunction(Node* line) {
	if(stack.empty()) {
		finished = true;
	} else {
		currentLine = stack.top();
		stack.pop();
	}

}
