#ifndef _PROGRAM
#define _PROGRAM

#include "Parser.h"
#include <stack>

#define SYNTAX Parser::parsedTokens()

class Program {
private:
	unsigned currentLine;
	std::stack<unsigned> stack;
	std::unordered_map<std::string, int> varsInt;
	std::unordered_map<std::string, char16_t> varsChar;
	std::unordered_map<std::string, float> varsFloat;
	std::unordered_map<std::string, std::string> varsText;
	std::unordered_map<std::string, VariableType> varTyping;

	unsigned getFuncDef (const std::string& name) const;
	bool finished;

	void allocVariable (Node* assign);
	void* getValue(const std::string & name, const VariableType& expectedType);
	bool testVariable(Node* test);
	void removeVar (Token* id);

	void loop (Node* line);
	void incrOrDecr (Node* line);
	void print(Node* line);
	void exitFunction(Node* line);
public:
	void run (const std::string& func);
};

#endif
