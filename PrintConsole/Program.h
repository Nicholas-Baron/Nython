#ifndef _PROGRAM
#define _PROGRAM

#include "Parser.h"
#include <stack>

class Program {
private:
	std::vector<Node*> syntax;
	unsigned currentLine;
	std::stack<unsigned> stack;
	std::unordered_map<std::pair<std::string, std::string>, void*> variables;

	unsigned getFuncDef (std::string name) const;
	bool finished;

	void allocVariable (Node* assign);
	void removeVar (Token* id);

	void loop (Node* line);
public:
	Program ();
	void run (std::string func);
};

#endif