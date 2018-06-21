#ifndef _PROGRAM
#define _PROGRAM

#include "ActionTree.h"
#include <unordered_map>
#include <stack>
#include <iostream>

class StackFrame {
private:
	std::unordered_map<std::string, VariableType> typeMap;
	std::unordered_map<std::string, bool> booleans;
	std::unordered_map<std::string, int> integers;
	std::unordered_map<std::string, float> floats;
	std::unordered_map<std::string, char> characters;
	std::unordered_map<std::string, std::string> text;

	inline VariableType getByName(const std::string& name) const {
		return typeMap.at(name);
	}

public:
	VariableType getTypeOf(const std::string& name) const;
	
	bool boolAtID(const std::string& name) const;
	int intAtID(const std::string& name) const;
	char charAtID(const std::string& name) const;
	float floatAtID(const std::string& name) const;
	std::string stringAtID(const std::string& name) const;

	void setBool(const std::string& name, bool val) {
		if(getTypeOf(name) != VariableType::BOOL) {
			std::cerr << "Another variable has the name " << name << std::endl;
			return;
		}
		booleans[name] = val;
	}
	void setInt(const std::string& name, int val) {
		if(getTypeOf(name) != VariableType::INT) {
			std::cerr << "Another variable has the name " << name << std::endl;
			return;
		}
		integers[name] = val;
	}
	void setChar(const std::string& name, char val) {
		if(getTypeOf(name) != VariableType::CHAR) {
			std::cerr << "Another variable has the name " << name << std::endl;
			return;
		}
		characters[name] = val;
	}
	void setFloat(const std::string& name, float val) {
		if(getTypeOf(name) != VariableType::FLOAT) {
			std::cerr << "Another variable has the name " << name << std::endl;
			return;
		}
		floats[name] = val;
	}
	void setString(const std::string& name, const std::string& val) {
		if(getTypeOf(name) != VariableType::STRING) {
			std::cerr << "Another variable has the name " << name << std::endl;
			return;
		}
		text[name] = val;
	}
};

struct FunctionReturn {
	VariableType type;
	void* location;
};

class Program {
private:
	ActionTree actions;
	std::stack<unsigned> currentExecution;
	std::stack<StackFrame> frames;

	inline Action* findTree(const std::string& name, unsigned& loc) const {
		for(unsigned i = 0; i < actions.actionList().size(); i++) {
			if(actions.actionList()[i]->tok->text == name && actions.actionList()[i]->type == ActionType::DEFINITION) {
				loc = i;
			}
		}
		return actions.actionList()[loc];
	}

public:
	Program(ActionTree actTree) : actions(actTree) {}
	FunctionReturn run(const std::string& funcName);
};

#endif