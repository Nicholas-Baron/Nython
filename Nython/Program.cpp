#include "Program.h"

VariableType StackFrame::getTypeOf(const std::string & name) const {

	if(typeMap.find(name) == typeMap.end()) {
		return VariableType::VOID;
	}
	return typeMap.at(name);
}

bool StackFrame::boolAtID(const std::string & name) const {
	auto info = getTypeOf(name);
	if(info == VariableType::BOOL) {
		return booleans.at(name);
	}
	std::cerr << name << " is not a boolean." << std::endl;
	return false;
}

int StackFrame::intAtID(const std::string & name) const {
	auto info = getTypeOf(name);
	if(info == VariableType::INT) {
		return integers.at(name);
	}
	std::cerr << name << " is not a integer." << std::endl;
	return 0;
}

char StackFrame::charAtID(const std::string & name) const {
	auto info = getTypeOf(name);
	if(info == VariableType::CHAR) {
		return characters.at(name);
	}
	std::cerr << name << " is not a char." << std::endl;
	return 0;
}

float StackFrame::floatAtID(const std::string & name) const {
	auto info = getTypeOf(name);
	if(info == VariableType::FLOAT) {
		return floats.at(name);
	}
	std::cerr << name << " is not a float." << std::endl;
	return 0;
}

std::string StackFrame::stringAtID(const std::string & name) const {
	auto info = getTypeOf(name);
	if(info == VariableType::STRING) {
		return text.at(name);
	}
	std::cerr << name << " is not a string." << std::endl;
	return 0;
}
//End StackFrame Implementation

FunctionReturn Program::processCall(Action* call) {
	const auto& funcName = call->tok->text;
	call->children.shrink_to_fit();

	if(Keywords::isCommand(funcName)) {
		if(funcName == "endline") {
			std::cout << std::endl;
		} else if(funcName == "print") {
			std::cout << doAction(call->children[0]);
		} else if(Keywords::isFuncEnd(call->tok)) {

			//std::cout << "[DEBUG] Returning from a function." << std::endl;

			currentExecution.top().second = true;
			if(call->hasChildren()) {
				return doAction(call->children[0]);
			} else {
				return FunctionReturn{call->resultType};
			}
		} else if(Keywords::isLoopStart(call->tok)) {
			doAction(call->children[0]);
			auto repeat = static_cast<bool>(doAction(call->children[1]));

			while(repeat) {
				for(size_t i = 3; i < call->children.size(); i++) {
					doAction(call->children[i]);
				}
				doAction(call->children[2]);
				repeat = static_cast<bool>(doAction(call->children[1]));
			}
		}

		return FunctionReturn{VariableType::VOID, nullptr};
	}

	//Run user-defined function
	unsigned defLocation = 0;
	const auto& definition = findTree(funcName, defLocation);

	//Ensure correctness of parameters
	unsigned numParameters = 0;
	for(size_t i = 0; i < definition->children.size(); i++) {
		if(definition->children[i]->type == ActionType::DEFINITION) {
			numParameters++;
		} else {
			break;
		}
	}

	//std::cout << "[TEMP] Function " << funcName << " has " << numParameters << " parameters." << std::endl;

	if(numParameters != call->children.size()) {
		std::cerr << "[ERR] Function " << funcName << " called with " << call->children.size() << " parameters, instead of " << numParameters << std::endl;
		std::cout << currentFrame() << std::endl;
		return FunctionReturn{definition->resultType, nullptr};
	}

	if(numParameters == 0) {
		return run(funcName);
	}

	StackFrame params;
	for(size_t i = 0; i < numParameters; i++) {
		auto defSide = definition->children[i];
		auto callSide = call->children[i];

		if(callSide->resultType != defSide->resultType) {
			std::cerr << "Wrong parameter type for " << defSide->tok->text << ": " << callSide->tok->text << std::endl;
		} else if(callSide->type == LITVAL) { //TODO: Merge with below if
			params.setVariable(defSide, callSide);
		} else if(callSide->type == OPCALL || callSide->type == CALL) {
			params.setVariable(defSide, doAction(callSide));
		} else {
			std::cerr << "Variable not set: " << defSide->tok->text << std::endl;
		}
	}

	return run(funcName, params);
}

FunctionReturn processLiteral(Action* value) {
	FunctionReturn toRet{value->resultType};

	const auto& text = value->tok->text;
	switch(value->resultType) {
		case VariableType::BOOL:
			toRet.location = new bool{Keywords::getBoolFromToken(value->tok)};
			break;
		case VariableType::INT:
			toRet.location = new int{std::stoi(text)};
			break;
		case VariableType::STRING:
			//Cuts off the beginning and ending quote marks
			toRet.location = new std::string{text.substr(1, text.size() - 2)};
			break;
		default:
			std::cerr << "Can not get a literal value from " << text << std::endl;
			break;
	}

	return toRet;
}

FunctionReturn Program::processOperator(Action* call) {

	FunctionReturn toRet;
	if(call->children.size() == 2) {
		const auto left = call->children[0], right = call->children[1];

		//Assignment
		if(Keywords::isAssignment(call->tok)) {

			toRet.type = VariableType::VOID;
			auto success = currentFrame().setVariable(left, right);
			if(!success) {
				std::cerr << "[ERR] Failed to set " << left->tok->text << " to " << right->tok->text << std::endl;
			}

		//Binary
		} else if(Keywords::isBinaryOp(call->tok)) {
			const auto resLeft = doAction(left);
			const auto resRight = doAction(right);
			const auto& op = call->tok->text;

			//True/False
			if(Keywords::isBoolOp(call->tok)) {

				toRet.type = VariableType::BOOL;

				if(resLeft.type == VariableType::INT) {
					auto left = static_cast<int>(resLeft);
					switch(resRight.type) {
						case VariableType::INT:
							toRet.location = new bool(Keywords::opTest(op, left, static_cast<int>(resRight)));
							break;
						case VariableType::FLOAT:
							toRet.location = new bool(Keywords::opTest(op, left, static_cast<float>(resRight)));
							break;
						default:
							std::cerr << "[ERR] Unsupported comparison!" << std::endl;
							break;
					}
				} else { std::cerr << "[ERR] Unimplemented operator action!" << std::endl; }
			} else {
				//Is math operation
				auto preferred = resLeft.type;

				if(preferred == VariableType::INT && resRight.type == VariableType::FLOAT) {
					preferred = resRight.type;
				}

				toRet.type = preferred;

				if(preferred == VariableType::INT) {
					switch(resRight.type) {
						case VariableType::INT:
							toRet.location = new int(Keywords::opMath(op, static_cast<int>(resLeft), static_cast<int>(resRight)));
							break;
						case VariableType::FLOAT:
							toRet.location = new float(Keywords::opMath(op, static_cast<float>(resLeft), static_cast<float>(resRight)));
							break;
						default:
							std::cerr << "Unsupported math operation!" << std::endl;
							break;
					}
				}

			}
		}
	} else if(Keywords::isUnaryOp(call->tok) && call->children.size() == 1) {

		auto var = call->children[0];
		const auto& varName = call->children[0]->tok->text;

		if(var->resultType == VariableType::BOOL && Keywords::isBoolOp(call->tok)) {

			auto val = static_cast<bool>(doAction(var));
			Keywords::opUnary(call->tok->text, val);
			toRet.type = VariableType::BOOL;
			toRet.location = new bool(val);

		} else if(var->type == ActionType::VARIABLE && Keywords::canUseIncre(var->resultType) && currentFrame().getTypeOf(varName) == var->resultType) {

			toRet.type = var->resultType;

			if(var->resultType == VariableType::INT) {
				auto val = currentFrame().intAtID(varName);
				Keywords::opUnary(call->tok->text, val);
				toRet.location = new int(val);
				currentFrame().setInt(varName, val);
			} else if(var->resultType == VariableType::FLOAT) {
				auto val = currentFrame().floatAtID(varName);
				Keywords::opUnary(call->tok->text, val);
				toRet.location = new float(val);
				currentFrame().setFloat(varName, val);
			}
		}
	}

	return toRet;
}

FunctionReturn Program::processVariable(Action * var) {

	FunctionReturn toRet{var->resultType};

	switch(toRet.type) {
		case VariableType::INT:
			toRet.location = new int(currentFrame().intAtID(var->tok->text));
			break;
		default:
			std::cerr << "[ERR] Unimplemented variable type used!" << std::endl;
			break;
	}

	return toRet;
}

FunctionReturn Program::processDecision(Action * call) {

	if(!Keywords::isConditionalStart(call->tok) && Keywords::isEndOfConditional(call->tok)) {
		return doAction(call->children[0]);
	}

	auto result = static_cast<bool>(doAction(call->children[0]));

	if(result && Keywords::isConditionalStart(call->tok) && !Keywords::isEndOfConditional(call->tok)) {
		return doAction(call->children[1]);
	} else if(result && Keywords::isConditionalStart(call->tok) && Keywords::isEndOfConditional(call->tok)) {
		return doAction(call->children[1]);
	}

	return FunctionReturn{VariableType::BOOL, new bool(false)};
}

FunctionReturn Program::doAction(Action* tree) {

	if(!tree->hasChildren()) {
		switch(tree->type) {
			case ActionType::CALL:
				return processCall(tree);
			case ActionType::LITVAL:
				return processLiteral(tree);
			case ActionType::VARIABLE:
				return processVariable(tree);
			case ActionType::OPCALL:
				std::cerr << "[ERR] Operator has no children?" << std::endl;
			default:
				std::cerr << "[ERR] Unimplemented childless action!" << std::endl;
				break;
		}
	}

	switch(tree->type) {
		case ActionType::CALL:
			return processCall(tree);
		case ActionType::OPCALL:
			return processOperator(tree);
		case ActionType::DEFINITION:
			FunctionReturn toRet;
			for(auto& child : tree->children) {
				if(child->type != ActionType::DEFINITION && !currentExecution.top().second) {
					toRet = doAction(child);
				}
			}
			return toRet;
		case ActionType::DECISION:
			return processDecision(tree);
		default:
			std::cerr << "[ERR] Unimplemented action w/ children!" << std::endl;
			break;
	}

	//Should never get here!
	std::cerr << "[ERR] Some action was not processed!" << std::endl;
	return FunctionReturn();
}

FunctionReturn Program::run(const std::string & funcName, StackFrame params) {

	unsigned startLoc = 0;
	auto func = findTree(funcName, startLoc);

	currentExecution.push(std::make_pair(startLoc, false));
	frames.push(params);

	auto result = doAction(func);
	currentExecution.pop();
	frames.pop();

	return result;
}

std::ostream & operator<<(std::ostream & lhs, const FunctionReturn & rhs) {
	switch(rhs.type) {
		case VariableType::BOOL:
			if(!static_cast<bool>(rhs)) {
				lhs << "FALSE";
			} else {
				lhs << "TRUE";
			}
			break;
		case VariableType::CHAR:
			lhs << static_cast<char>(rhs);
			break;
		case VariableType::FLOAT:
			lhs << static_cast<float>(rhs);
			break;
		case VariableType::INT:
			lhs << static_cast<int>(rhs);
			break;
		case VariableType::STRING:
			lhs << static_cast<std::string>(rhs);
			break;
		default:
			lhs << "VOID";
			break;
	}
	return lhs;
}

std::ostream& operator<<(std::ostream& lhs, const StackFrame& rhs) {

	auto vars = rhs.currentVariables();

	for(const auto& var : vars) {

		lhs << std::endl << var.first << " : ";

		switch(var.second) {
			case VariableType::BOOL:
				if(rhs.boolAtID(var.first)) {
					lhs << "TRUE";
				} else {
					lhs << "FALSE";
				}
				break;
			case VariableType::INT:
				lhs << rhs.intAtID(var.first);
				break;
			default:
				lhs << "[ERR] Unimplemented variable type: " << var.second;
				break;
		}
	}

	return lhs;
}