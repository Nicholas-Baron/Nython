#ifndef _PROGRAM
#define _PROGRAM

#include "Parser.h"
#include <stack>

class Program {
private:
	std::vector<Node*> lines;
	unsigned currentLine;
	std::stack<unsigned> stack;
	VariableType mainExpectedReturn = VOID;
	
	std::unordered_map<std::string, int> varsInt;
	std::unordered_map<std::string, char16_t> varsChar;
	std::unordered_map<std::string, float> varsFloat;
	std::unordered_map<std::string, std::string> varsText;
	std::unordered_map<std::string, VariableType> varTyping;

	int getFuncDef (const std::string& name);
	bool finished;

	void allocVariable (Node* assign);
	void* getValue(Token* id, const VariableType& expectedType);
	bool testVariable(Node* test);
	bool variableExists(Token* id, bool printError = true) const;
	void removeVar (Token* id);

	void loop (Node* line);
	void incrOrDecr (Node* line);
	void print(Node* line);
	void processCommand(Node* line);
	void exitFunction(Node* line);
public:
	Program(const Parser& tree) :lines(tree.parsedTokens()) {}
	void run (const std::string& func);
};

#endif
