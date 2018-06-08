#ifndef _PROGRAM
#define _PROGRAM

#include "Parser.h"
#include <stack>

#define SYNTAX Parser::parsedTokens()

struct pair_hash {
	template<class T1, class T2>
	std::size_t operator()(const std::pair<T1, T2>& pair)const {
		return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
	}
};

class Program {
private:
	unsigned currentLine;
	std::stack<unsigned> stack;
	std::unordered_map<std::pair<VariableType, std::string>, void*, pair_hash> variables;
	std::unordered_map<std::string, VariableType> varTyping;

	unsigned getFuncDef (const std::string& name) const;
	bool finished;

	void allocVariable (Node* assign);
	void* getValue(const std::string & name, const VariableType& expectedType);
	bool testVariable(Node* test);
	void removeVar (Token* id);

	void loop (Node* line);
	void print(Node* line);
	void exitFunction(Node* line);
public:
	void run (const std::string& func);
};

#endif
