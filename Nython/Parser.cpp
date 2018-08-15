#include "Parser.h"

#include <algorithm>

//TODO: move these into the class
void parseOperator(TOKEN_LIST, Node* addTo, unsigned& pos);
void parseCommand(TOKEN_LIST, Node* comm, unsigned& pos, Token* next);
void parseSequence(TOKEN_LIST, Node* addTo, unsigned& start, unsigned end, bool multiLine);
void parseIdentifier(TOKEN_LIST, Node* id, unsigned& pos, Token* next);
Node* parseToken(TOKEN_LIST, Token* t, unsigned& pos, Token* next, bool isLast);

Node* createNode(Token* t) {
	Node* toRet = new Node;
	toRet->token = t;
	return toRet;
}

unsigned nextOfType(TOKEN_LIST, unsigned start, TokenType type, bool walkBackwards = false) {
	unsigned toRet = start;
	int depth = 0;
	Token* current = NULL;
	if(walkBackwards) {
		toRet--;
		current = tokenList[toRet];
		while((current->type != type || depth != 0) && toRet > 0) {
			if(Keywords::isSecondOfPairDelin(current)) {
				depth++;
			} else if(Keywords::isFirstOfPairDelin(current)) { depth--; }

			toRet--;
			current = tokenList[toRet];
		}
	} else {
		toRet++;
		current = tokenList[toRet];
		while((current->type != type || depth != 0) && toRet < tokenList.size() - 1) {
			if(Keywords::isSecondOfPairDelin(current)) {
				depth--;
			} else if(Keywords::isFirstOfPairDelin(current)) { depth++; }

			toRet++;
			current = tokenList[toRet];
		}
	}
	return toRet;
}

void Parser::readNode(Node* root, int depth) {
	if(root != nullptr) {
		std::cout << "[" << depth << "]" << *(root->token) << " ";

		if(root->children.size() >= 1) {
			for(unsigned i = 0; i < root->children.size(); i++) {
				readNode(root->children[i], depth + 1);
			}
		}
	}
}

//Finds the closing parenthesis
Token* otherEndOfPair(TOKEN_LIST, unsigned start, unsigned& end, bool walkBackwards = false) {
	Token* toRet = NULL;
	end = start;
	unsigned depth = 0;

	if(walkBackwards) {
		end--;
	} else {
		end++;
	}

	while(toRet == NULL) {
		const auto current = tokenList[end];
		if(Keywords::isSecondOfPairDelin(current) && depth == 0) {
			toRet = tokenList[end];
		} else {
			if(Keywords::isFirstOfPairDelin(current)) {
				depth++;
			} else if(Keywords::isSecondOfPairDelin(current) && depth != 0) {
				depth--;
			}

			if(walkBackwards) {
				end--;
			} else {
				end++;
			}
		}
	}

	return toRet;
}

//Finds the end of a repeat statement
Token* endOfLoop(TOKEN_LIST, unsigned start, unsigned& end) {
	Token* toRet = NULL;
	end = start + 1;

	while(toRet == NULL) {
		if(Keywords::isEndOfLoop(tokenList[end])) {
			toRet = tokenList[end];
		} else { end++; }
	}

	return toRet;
}

//Finds the end of an if statement
Token* endOfIf(TOKEN_LIST, unsigned start, unsigned& end) {
	Token* toRet = NULL;
	end = start + 1;

	while(toRet == NULL) {
		if(Keywords::isEndOfConditional(tokenList[end])) {
			toRet = tokenList[end];
		} else { end++; }
	}

	return toRet;
}

void parseOperator(TOKEN_LIST, Node* addTo, unsigned& pos) {
	auto isBin = Keywords::isBinaryOp(tokenList[pos]);
	auto leftLoc = nextOfType(tokenList, pos, TokenType::IDENTIFIER, true);

	auto prevNode = !Keywords::isAssignment(addTo->token) && tokenList[pos - 1]->type == TokenType::DELINEATOR
		? parseToken(tokenList, tokenList[leftLoc], leftLoc, tokenList[leftLoc + 1], false)
		: createNode(tokenList[leftLoc]);

	if(isBin) {
		auto nextOp = nextOfType(tokenList, pos, TokenType::OPERATOR);
		bool nextOpUsed = false;

		Node* nextNode = nullptr;
		if(nextOfType(tokenList, pos, TokenType::TYPE) > nextOp && nextOfType(tokenList, pos, TokenType::COMMAND) > nextOp
		   && tokenList[pos + 2]->type == TokenType::OPERATOR) {
			nextNode = createNode(tokenList[nextOp]);
			parseOperator(tokenList, nextNode, nextOp);
			nextOpUsed = true;
		} else {
			nextNode = createNode(tokenList[pos + 1]);
		}
		//The type name is two spaces behind the current location
		if(tokenList[pos - 2]->type == TokenType::TYPE && Keywords::isAssignment(addTo->token)) {
			prevNode->children.push_back(createNode(tokenList[pos - 2]));
		}

		if(Keywords::isFirstOfPairDelin(tokenList[pos + 2])) {
			pos++;
			parseIdentifier(tokenList, nextNode, pos, tokenList[pos + 1]);
		}

		addTo->children.push_back(prevNode);
		addTo->children.push_back(nextNode);
		if(nextOpUsed) {
			pos = nextOp;
		} else if(nextNode->children.size() > 1) {
			pos += nextNode->children.size();
		} else {
			pos++;
		}
	} else {
		addTo->children.push_back(prevNode);
	}
}

//Used for command and method parameters
void parseAfterDelineator(TOKEN_LIST, Node* delin, unsigned pos) {
	unsigned finish = pos + 1;
	while(tokenList[finish]->type != TokenType::DELINEATOR) {
		finish++;
	}

	for(unsigned i = pos + 1; i < finish; i++) {
		auto t = tokenList[i];
		auto next = tokenList[i + 1];

		if(t->type == TokenType::OPERATOR) {
			Node* op = createNode(t);
			parseOperator(tokenList, op, i);
			delin->children.push_back(op);
		} else if(next->type == TokenType::OPERATOR) {
			i++;
			Node* op = createNode(next);
			parseOperator(tokenList, op, i);
			delin->children.push_back(op);
		} else if(t->type == TokenType::IDENTIFIER) {
			Node* id = createNode(t);
			parseIdentifier(tokenList, id, i, tokenList[i + 1]);
			delin->children.push_back(id);
		} else if(t->type == TokenType::LITERAL) {
			Node* id = createNode(t);
			delin->children.push_back(id);
		}
	}
}

void parseCommand(TOKEN_LIST, Node* comm, unsigned& pos, Token* next) {
	if(next != nullptr && next->type == TokenType::DELINEATOR && Keywords::isFirstOfPairDelin(next)) {
		auto endOfPairPos = pos + 2;
		auto otherEnd = otherEndOfPair(tokenList, pos + 1, endOfPairPos);

		int depth = 0;
		for(unsigned j = pos + 1; j < endOfPairPos; j++) {
			const auto current = tokenList[j];

			if((current->type == TokenType::DELINEATOR && depth == 0) || Keywords::nonPairedDelin(current)) {
				const auto delin = createNode(current);
				parseAfterDelineator(tokenList, delin, j);
				comm->children.push_back(delin);
			}

			if(Keywords::isFirstOfPairDelin(current)) {
				depth++;
			} else if(Keywords::isSecondOfPairDelin(current)) {
				depth--;
			}
		}

		Node* delinEnd = createNode(otherEnd);
		comm->children.push_back(delinEnd);

		pos = endOfPairPos;

		if(Keywords::isLoopStart(comm->token)) {
			unsigned endLoopPos = pos;
			Token* endLoop = endOfLoop(tokenList, pos, endLoopPos);
			pos++;
			parseSequence(tokenList, delinEnd, pos, endLoopPos, true);

			Node* loopEnd = createNode(endLoop);
			delinEnd->children.push_back(loopEnd);

			pos = endLoopPos;
		} else if(Keywords::isConditionalStart(comm->token)) {
			unsigned endIfPos = pos;
			endOfIf(tokenList, pos, endIfPos);
			pos++;
			parseSequence(tokenList, delinEnd, pos, endIfPos, true);

			pos = endIfPos - 1;
		}
	} else {
		if(Keywords::needsParameter(comm->token)) {
			pos++;
			Node* param = parseToken(tokenList, next, pos, tokenList[pos + 1], pos + 1 == tokenList.size());
			comm->children.push_back(param);
		} else if(Keywords::isFuncEnd(comm->token)) {
			if(next == nullptr) {
				//A blank return (ends a void function)
				return;
			} else if(next->type == TokenType::LITERAL) {
				Node* param = createNode(next);
				comm->children.push_back(param);
				pos++;
			} else if(next->type == TokenType::IDENTIFIER) {
				pos++;
				auto nextCmd = nextOfType(tokenList, pos, TokenType::COMMAND);
				auto nextType = nextOfType(tokenList, pos, TokenType::TYPE);

				unsigned stop = std::min(nextCmd, nextType);
				parseSequence(tokenList, comm, pos, stop, false);
				pos = stop;
			}
		}
	}
}

//Reads a sequence of tokens for loops, if-statements, or long returns
void parseSequence(TOKEN_LIST, Node* addTo, unsigned& start, unsigned end, bool multiLine) {
	auto nextOp = nextOfType(tokenList, start, TokenType::OPERATOR);

	if(nextOp < end && !multiLine) {
		nextOp--; //Moves it one back
		auto op = parseToken(tokenList, tokenList[nextOp], nextOp, tokenList[nextOp + 1], nextOp + 1 == end);
		addTo->children.push_back(op);
	} else {
		for(unsigned i = start; i < end; i++) {
			auto parsed = parseToken(tokenList, tokenList[i], i, tokenList[i + 1], i + 1 == end);
			if(parsed != nullptr) {
				addTo->children.push_back(parsed);
			}
		}
	}
}

//Identifier is either a variable name or method name
void parseIdentifier(TOKEN_LIST, Node* id, unsigned& pos, Token* next) {
	auto prev = tokenList[pos - 1];

	if(prev->type == TokenType::TYPE) {
		auto prevNode = createNode(prev);
		id->children.push_back(prevNode);
	}

	if(next->type == TokenType::DELINEATOR && Keywords::isFirstOfPairDelin(next)) {
		unsigned endOfPair = pos + 2;
		pos++;
		otherEndOfPair(tokenList, pos, endOfPair);

		auto nextDelin = nextOfType(tokenList, pos, TokenType::DELINEATOR);
		while(nextDelin <= endOfPair) {
			Node* delin = createNode(tokenList[pos]);
			parseAfterDelineator(tokenList, delin, pos);
			id->children.push_back(delin);
			pos = nextDelin;
			nextDelin = nextOfType(tokenList, pos, TokenType::DELINEATOR);
		}

		pos = endOfPair;
	}
}

//Parses a single token
Node* parseToken(TOKEN_LIST, Token* t, unsigned& pos, Token* next, bool isLast) {
	Node* toRet = NULL;

	if(t->type == TokenType::COMMAND) {
		toRet = createNode(t);
		parseCommand(tokenList, toRet, pos, next);
	} else if(!isLast && next->type == TokenType::OPERATOR) {
		toRet = createNode(next);
		pos++;
		parseOperator(tokenList, toRet, pos);
	} else if(t->type == TokenType::IDENTIFIER) {
		toRet = createNode(t);
		parseIdentifier(tokenList, toRet, pos, next);
	} else if(t->type == TokenType::LITERAL) {
		toRet = createNode(t);
	}

	return toRet;
}

void Parser::trimRoots() {
	std::vector<Node*> temp;
	for(unsigned i = 0; i < roots.size(); i++) {
		auto t = roots[i];
		if(t) {
			temp.push_back(t);
		}
	}
	roots = temp;
}

Parser::Parser(TOKEN_LIST) {
	roots = parseTokens(tokenList);
	trimRoots();
	roots.shrink_to_fit();
	ready = true;
}

std::vector<Node*> Parser::parseTokens(TOKEN_LIST) {
	std::vector<Node*> branches;
	for(unsigned i = 0; i < tokenList.size(); i++) {
		auto t = tokenList[i];

		bool isLast = i + 1 == tokenList.size();
		Token* next = NULL;
		if(!isLast) {
			next = tokenList[i + 1];
		}

		branches.push_back(parseToken(tokenList, t, i, next, isLast));
	}
	return branches;
}
