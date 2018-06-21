#include "Program.h"

VariableType StackFrame::getTypeOf(const std::string & name) const {
	
	if(typeMap.find(name) == typeMap.end()) {
		return VariableType::VOID;
	}
	return typeMap.at(name);
}

bool StackFrame::boolAtID(const std::string & name) const {
	auto info = getByName(name);
	if(info == VariableType::BOOL) {
		return booleans.at(name);
	}
	std::cerr << name << " is not a boolean." << std::endl;
	return false;
}

int StackFrame::intAtID(const std::string & name) const {
	auto info = getByName(name);
	if(info == VariableType::INT) {
		return integers.at(name);
	}
	std::cerr << name << " is not a integer." << std::endl;
	return 0;
}

char StackFrame::charAtID(const std::string & name) const {
	auto info = getByName(name);
	if(info == VariableType::CHAR) {
		return characters.at(name);
	}
	std::cerr << name << " is not a char." << std::endl;
	return 0;
}

float StackFrame::floatAtID(const std::string & name) const {
	auto info = getByName(name);
	if(info == VariableType::FLOAT) {
		return floats.at(name);
	}
	std::cerr << name << " is not a float." << std::endl;
	return 0;
}

std::string StackFrame::stringAtID(const std::string & name) const {
	auto info = getByName(name);
	if(info == VariableType::STRING) {
		return text.at(name);
	}
	std::cerr << name << " is not a string." << std::endl;
	return 0;
}

//End StackFrame Implementation




FunctionReturn doAction(Action* tree) {
	auto firstToExec = tree->children[0];
	if(firstToExec != nullptr) {
		switch(firstToExec->type) {
			case ActionType::CALL:
			case ActionType::DECISION:

			default:
				break;
		}
	}
}

FunctionReturn Program::run(const std::string & funcName) {
	unsigned loc = 0;
	auto func = findTree(funcName, loc);
	currentExecution.push(loc);
	return doAction(func);
	
}
