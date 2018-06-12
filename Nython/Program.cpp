#include "Program.h"
#include <iostream>
#include <string>

int Program::getFuncDef (const std::string& name) {

	int start = -1 * lines.size();
	for (unsigned i = 0; i < lines.size(); i++) {
		auto nod = lines[i];
		if(nod) {
			if(nod->token->text == name&&Parser::isDeclaration(nod)) {
				start = i;
				if(name == "main") {
					mainExpectedReturn = Keywords::getVarType(*(nod->children[0]->token));
				}
			}
		}
	}

	if(start < 0) {
		std::cerr << "Could not find function " << name << std::endl;
	}

	return start;
}

void Program::allocVariable (Node * assign) { 

	if(variableExists(assign->children[0]->token, false)) {
		std::cerr << "Please use a = without declaring the type" << std::endl;
		return;
	}
	std::string name = assign->children[0]->token->text;
	VariableType type = Keywords::getVarType(*(assign->children[0]->children[0]->token));
	varTyping[name] = type;
	
	std::string value = assign->children[1]->token->text;
	switch (type) {
		case INT:
			varsInt[name] = stoi (value);
			break;
		case STRING:
			varsText[name] = value;
			break;
		case FLOAT:
			varsFloat[name] = stof (value);
			break;
		case CHAR:
			varsChar[name] = value[1];
		case VOID:
		default:
			break;
	}
}

bool Program::variableExists(Token* id, bool printError) const {
	if(varTyping.find(id->text) == varTyping.end()) {
		if(printError) {
			std::cerr << "Line #" << id->line_number + 1 << ": ";
			std::cerr << "Variable " << id->text << " does not exist!" << std::endl;
		}
		return false;
	}
	return true;
}

void * Program::getValue(Token* id, const VariableType& expectedType) {
	if(!variableExists(id)) {
		return nullptr;
	}
	
	auto type = varTyping[id->text];
	if(expectedType != type) {
		std::cout << "Unsupported action!" << std::endl;
		return nullptr;
	}
	switch (expectedType) {
		case INT:
			return &varsInt[id->text];
		case FLOAT:
			return &varsFloat[id->text];
		case CHAR:
			return &varsChar[id->text];
		case STRING:
			return &varsText[id->text];
		case VOID:
		default:
			return NULL;
	}
}
/*two levels of pointers probally should improve/find another way*/
bool Program::testVariable(Node * test) {
	auto txt = test->token->text;
	auto left = test->children[0];
	auto right = test->children[1];
	
	auto leftType = VariableType::VOID;
	if (left->token->type == TokenType::IDENTIFIER) {
		if(variableExists(left->token)) {
			leftType = varTyping[left->token->text];
		} 
	}
	
	auto rightType = VariableType::VOID;
	if (right->token->type == TokenType::IDENTIFIER) {
		if(variableExists(right->token)) {
			rightType = varTyping[right->token->text];
		} 
	}

	if (leftType == VariableType::FLOAT || rightType == VariableType::FLOAT) { 
		float l, r;
		if (left->token->type == TokenType::IDENTIFIER) {
			l = *(float*)getValue (left->token, leftType);
		} else if (left->token->type == TokenType::LITERAL) {
			l = stof (left->token->text);
		}

		if (right->token->type == TokenType::IDENTIFIER) {
			r = *(float*) getValue (right->token, leftType);
		} else if (right->token->type == TokenType::LITERAL) {
			r = stof (right->token->text);
		}

		return Keywords::opProcess (txt, l, r);

	} else if (leftType == VariableType::INT || rightType == VariableType::INT) {
		int l, r;
		if (left->token->type == TokenType::IDENTIFIER) {
			l = *(int*) getValue (left->token, leftType);
		} else if (left->token->type == TokenType::LITERAL) {
			l = stoi (left->token->text);
		}

		if (right->token->type == TokenType::IDENTIFIER) {
			r = *(int*) getValue (right->token, leftType);
		} else if (right->token->type == TokenType::LITERAL) {
			r = stoi (right->token->text);
		}

		return Keywords::opProcess (txt, l, r);
	}

	return false;
}

void Program::removeVar (Token * id) { 
	auto name = id->text;
	if(!variableExists(id)) {
		return;
	}
	switch (varTyping[name]) {
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
		auto line = lines[currentLine];
		auto tokType = line->token->type;
		auto tokTxt = line->token->text;

		if (tokType == TokenType::IDENTIFIER) {
			stack.push (currentLine);
			run (tokTxt);
		} else if(tokType == TokenType::COMMAND){
			processCommand(line);
		}

		currentLine++;
	}

}

void Program::incrOrDecr (Node* line) { 

	auto txt = line->token->text;
	auto var = line->children[0]->token->text;

	if (txt == "++") {
		auto type = varTyping[var];
		if (type == VariableType::INT) {
			varsInt[var]++;
		} else if (type == VariableType::FLOAT) {
			varsFloat[var]++;
		}
	} else if (txt == "--") {
		auto type = varTyping[var];
		if (type == VariableType::INT) {
			varsInt[var]--;
		} else if (type == VariableType::FLOAT) {
			varsFloat[var]--;
		}
	}
}

void Program::loop (Node* line) { 

	auto testNode = (line->children.size() == 4) ? line->children[1]->children[0] : line->children[0]->children[0];

	if (line->children.size() == 4) {
		auto initializer = line->children[0]->children[0];
		allocVariable (initializer);
		while(testVariable(testNode)) {
			for (unsigned i = 0; i < line->children[3]->children.size ( );i++ ) {
				auto tokType = line->children[3]->children[i]->token->type;
				auto tokTxt = line->children[3]->children[i]->token->text;
				if (tokType == TokenType::IDENTIFIER) {
					stack.push (currentLine);
					run (tokTxt);
				} else if (tokType == TokenType::COMMAND) {
					processCommand(line->children[3]->children[i]);
				}
			}
			incrOrDecr (line->children[2]->children[0]);
		}
		removeVar (initializer->children[0]->token);
	}
}

void Program::processCommand(Node* line) {
	auto tokTxt = line->token->text;
	if(tokTxt == "repeat") {
		loop(line);
	} else if(tokTxt == "ret" || tokTxt == "return") {
		exitFunction(line);
	} else if(tokTxt == "print") {
		print(line);
	} else if(tokTxt == "endline") { std::cout << std::endl; }
}

void Program::print(Node * line) {
	auto tok = line->children[0]->token;

	if(tok->type == LITERAL) {
		std::cout << tok->text;
	} else if (tok->type == IDENTIFIER && variableExists(tok)) {
		
		auto type = varTyping[tok->text];
		if (type == VariableType::FLOAT) {
			float val = varsFloat[tok->text];
			std::cout << val;
		} else if (type == VariableType::INT) {
			int val = varsInt[tok->text];
			std::cout << val;
		}
	}
}

void Program::exitFunction(Node* line) {
	if(stack.empty()) {
		finished = true;
		auto tokToRet = line->children[0]->token;
		if(tokToRet->type == IDENTIFIER) {
			switch(mainExpectedReturn) {
				case INT:
					std::cout << "Main returned " << *((int*)getValue(tokToRet, mainExpectedReturn)) << std::endl;
					break;
				case STRING:
					std::cout << "Main returned " << *((std::string*) getValue(tokToRet, mainExpectedReturn)) << std::endl;
					break;
				case VOID:
				default:
					break;

			}
		} else if(tokToRet->type == LITERAL) {
			switch(mainExpectedReturn) {
				case INT:
					std::cout << "Main returned " << stoi(tokToRet->text) << std::endl;
					break;
				case STRING:
					std::cout << "Main returned " << tokToRet->text << std::endl;
					break;
				case VOID:
				default:
					break;

			}
		}
	} else {
		currentLine = stack.top();
		stack.pop();
	}

}
