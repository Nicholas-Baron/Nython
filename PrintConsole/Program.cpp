#include "Program.h"

unsigned Program::getFuncDef (std::string name) const {

	unsigned start = 0;
	for (unsigned i = 0; i < syntax.size(); i++) {
		if (syntax[i]->token->token == name) {
			start = i;
		}
	}

	return start;
}

void Program::allocVariable (Node * assign) { 

	std::string name = assign->children[0]->token->token;
	std::string type = assign->children[0]->children[0]->token->token;
	void* value = assign->children[1]->token->token;
}

void Program::removeVar (Token * id) { }

Program::Program () : syntax(Parser::parsedTokens()) {


}

void Program::run (std::string func) { 

	currentLine = getFuncDef (func) + 1;
	
	while (!finished) {
		auto line = syntax[currentLine];
		auto tokType = line->token->type;

		if (tokType == TokenType::IDENTIFIER) {
			stack.push (currentLine);
			run (line->token->token);
			stack.pop ( );
		} else if(tokType == TokenType::COMMAND){
			if (line->token->token == "repeat") {
				loop (line);
			}
		}

		currentLine++;
	}

}

void Program::loop (Node* line) { 

	if (line->children.size() == 4) {
		allocVariable (line->children[0]->children[0]);

	}

}
