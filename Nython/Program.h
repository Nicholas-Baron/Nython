#ifndef _PROGRAM
#define _PROGRAM

#include "ActionTree.h"
#include <unordered_map>
#include <stack>
#include <iostream>

struct FunctionReturn {
	VariableType type;
	void* location;

	operator bool() const {
		if(type != VariableType::BOOL) {
			std::cerr << "Converting " << type << " to a bool" << std::endl;
		}
		return *static_cast<bool*>(location);
	}

	operator int() const {
		if(type != VariableType::INT) {
			std::cerr << "Converting " << type << " to an int" << std::endl;
		}
		return *static_cast<int*>(location);
	}

	operator float() const {
		if(type != VariableType::FLOAT) {
			std::cerr << "Converting " << type << " to a float" << std::endl;
		}
		return *static_cast<float*>(location);
	}
};

std::ostream& operator<<(std::ostream& lhs, const FunctionReturn& rhs);

class StackFrame {
private:
	std::unordered_map<std::string, VariableType> typeMap;
	std::unordered_map<std::string, bool> booleans;
	std::unordered_map<std::string, int> integers;
	std::unordered_map<std::string, float> floats;
	std::unordered_map<std::string, char> characters;
	std::unordered_map<std::string, std::string> text;

public:
	VariableType getTypeOf(const std::string& name) const;
	
	bool boolAtID(const std::string& name) const;
	int intAtID(const std::string& name) const;
	char charAtID(const std::string& name) const;
	float floatAtID(const std::string& name) const;
	std::string stringAtID(const std::string& name) const;

	std::vector<std::pair<std::string, VariableType>> currentVariables() const {
		std::vector<std::pair<std::string, VariableType>> toRet;
		for(auto it = typeMap.begin(); it != typeMap.end(); ++it) {
			toRet.push_back(std::make_pair(it->first, it->second));
		}
		return toRet;
	}

	void setBool(const std::string& name, bool val) {
		if(getTypeOf(name) != VariableType::BOOL) {
			std::cerr << "A not bool variable has the name " << name << std::endl;
			return;
		}
		booleans[name] = val;
		typeMap[name] = VariableType::BOOL;
	}
	void setInt(const std::string& name, int val) {
		if(getTypeOf(name) != VariableType::INT) {
			std::cerr << "A not int variable has the name " << name << " (Registered as "<< getTypeOf(name)<< ")" << std::endl;
			return;
		}
		integers[name] = val; 
		typeMap[name] = VariableType::INT;
	}
	void setChar(const std::string& name, char val) {
		if(getTypeOf(name) != VariableType::CHAR) {
			std::cerr << "A not char variable has the name " << name << std::endl;
			return;
		}
		characters[name] = val;
		typeMap[name] = VariableType::CHAR;
	}
	void setFloat(const std::string& name, float val) {
		if(getTypeOf(name) != VariableType::FLOAT) {
			std::cerr << "A not float variable has the name " << name << std::endl;
			return;
		}
		floats[name] = val;
		typeMap[name] = VariableType::CHAR;
	}
	void setString(const std::string& name, const std::string& val) {
		if(getTypeOf(name) != VariableType::STRING) {
			std::cerr << "A not string variable has the name " << name << std::endl;
			return;
		}
		text[name] = val;
		typeMap[name] = VariableType::STRING;
	}

	bool setVariable(const Action* def, const Action* val) {
	
		const auto& defName = def->tok->text;

		if(def->type != DEFINITION || val->type != LITVAL || val->resultType != def->resultType) {
			std::cerr << "Could not set " << defName << " to value " << val->tok->text << std::endl;
			return false;
		}

		typeMap[defName] = def->resultType;

		switch(def->resultType) {
			case VariableType::BOOL:
				setBool(defName, Keywords::getBoolFromToken(val->tok));
				break;
			case VariableType::INT:
				setInt(defName, std::stoi(val->tok->text)); 
				break;
			case VariableType::VOID:
			default:
				std::cerr << "Variable type either unimplemented or is void!" << std::endl;
				return false;
		}

		return true;
	}
	bool setVariable(const Action* def, const FunctionReturn& val) {
	
		const auto& defName = def->tok->text;

		if(def->type != DEFINITION || val.type != def->resultType) {
			std::cerr << defName << " could not be set! (FunctionReturn)" << std::endl;
			return false;
		}
		
		typeMap[defName] = def->resultType;
		
		switch(def->resultType) {
			case VariableType::BOOL:
				setBool(defName, *static_cast<bool*>(val.location));
				//std::cout << defName << " now [DEBUG] is " << boolAtID(defName) << std::endl;
				break;
			case VariableType::INT:
				setInt(defName, *static_cast<int*>(val.location));
				//std::cout << defName << " now [DEBUG] is " << intAtID(defName) << std::endl;
				break;
			case VariableType::VOID:
			default:
				std::cerr << "Variable type either unimplemented or is void! (FunctionReturn)" << std::endl;
				return false;
		}

		return true;
	}
};

std::ostream& operator<<(std::ostream& lhs, const StackFrame& rhs);

class Program {
private:
	const ActionTree& actions;
	std::stack<std::pair<unsigned, bool>> currentExecution;
	std::stack<StackFrame> frames;

	inline Action* findTree(const std::string& name, unsigned& loc) const {
		for(unsigned i = 0; i < actions.actionList().size(); i++) {
			if(actions.actionList()[i]->tok->text == name && actions.actionList()[i]->type == ActionType::DEFINITION) {
				loc = i;
			}
		}
		return actions.actionList()[loc];
	}
	inline StackFrame& currentFrame() { return frames.top(); }

	FunctionReturn doAction(Action* tree);
	FunctionReturn processCall(Action* call);
	FunctionReturn processOperator(Action* call);
	FunctionReturn processVariable(Action* var);
	FunctionReturn processDecision(Action* call);

public:
	Program(const ActionTree& actTree) : actions(actTree) {}
	FunctionReturn run(const std::string& funcName, StackFrame parameters = StackFrame());
};

#endif