#include "Parser.h"

#include <iostream>

void parseOperator(TOKEN_LIST, Node* addTo, unsigned& pos);
void parseCommand(TOKEN_LIST, Node* comm, unsigned& pos, Token* next);
void parseLoop(TOKEN_LIST, Node* addTo, unsigned start, unsigned end);
void parseIdentifier(TOKEN_LIST, Node* id, unsigned pos);
Node* parseToken(TOKEN_LIST, Token* t, unsigned& pos, Token* next, bool isLast);

Node* createNode(Token* t) { 
	Node* toRet = new Node; 
	toRet->token = t; 
	return toRet;
}

void readNode(Node* root, int depth = 0) {

	if(root != nullptr) {
		std::cout << "[" << depth << "]" << *(root->token) << " ";

		if(root->children.size() >= 1) {
			for(unsigned i = 0; i < root->children.size(); i++) {
				readNode(root->children[i], depth + 1);
			}
		}
	}
}

Token* otherEndOfPair(TOKEN_LIST, unsigned start, unsigned& end) {
	Token* toRet = NULL;
	end = start + 1;

	while(toRet == NULL) {
		if(Keywords::isSecondOfPairDelin(*tokenList[end])) {
			toRet = tokenList[end];
		} else { end++; }
	}

	return toRet;
}

Token* otherEndOfLoop(TOKEN_LIST, unsigned start, unsigned& end) {
	Token* toRet = NULL;
	end = start + 1;

	while(toRet == NULL) {
		if(Keywords::isSecondOfPairLoop(*tokenList[end])) {
			toRet = tokenList[end];
		} else { end++; }
	}

	return toRet;
}

void parseOperator(TOKEN_LIST, Node* addTo, unsigned& pos) {
	auto isBin = Keywords::isBinaryOp(*tokenList[pos]);
	auto prevNode = createNode(tokenList[pos - 1]);
	if(isBin) {	
		auto nextNode = createNode(tokenList[pos + 1]);

		if(tokenList[pos-2]->type == TokenType::TYPE) {
			auto type = createNode(tokenList[pos - 2]);
			prevNode->children.push_back(type);
		}

		addTo->children.push_back(prevNode);
		addTo->children.push_back(nextNode);
		pos++;
	} else {
		addTo->children.push_back(prevNode);
	}
}

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
		} else if(t->type == TokenType::IDENTIFIER){
			Node* id = createNode(t);
			parseIdentifier(tokenList, id, i);
			delin->children.push_back(id);
		}
	}
}

void parseCommand(TOKEN_LIST, Node* comm, unsigned& pos, Token* next) {

	if(next->type == TokenType::DELINEATOR && Keywords::isFirstOfPairDelin(*next)) {

		unsigned endOfPair = pos;
		Token* otherEnd = otherEndOfPair(tokenList, pos, endOfPair);

		for(unsigned j = pos + 1; j < endOfPair; j++) {
			if(tokenList[j]->type == TokenType::DELINEATOR) {
				Node* delin = createNode(tokenList[j]);
				comm->children.push_back(delin);
				parseAfterDelineator(tokenList, delin, j);
			}
		}

		Node* delinEnd = createNode(otherEnd);
		comm->children.push_back(delinEnd);

		pos = endOfPair;

		if(Keywords::isLoopStart(*(comm->token))) {
			unsigned endOfLoop = pos;
			Token* endLoop = otherEndOfLoop(tokenList, endOfPair, endOfLoop);

			parseLoop(tokenList, delinEnd, endOfPair + 1, endOfLoop);

			Node* loopEnd = createNode(endLoop);
			delinEnd->children.push_back(loopEnd);

			pos = endOfLoop;
		}
	} else {
	
		if(Keywords::needsParameter(*(comm->token))) {
			Node* param = createNode(next);
			comm->children.push_back(param);
			pos++;
		} else if(Keywords::isFuncEnd(*(comm->token))) {
			
		}
	
	}
}

void parseLoop(TOKEN_LIST, Node* addTo, unsigned start, unsigned end) {

	for(unsigned i = start; i < end; i++) {
		addTo->children.push_back(parseToken(tokenList, tokenList[i],i,tokenList[i+1],i+1 == end));
	}
}

void parseIdentifier(TOKEN_LIST, Node* id, unsigned pos) {

	auto prev = tokenList[pos - 1];

	if(prev->type==TokenType::TYPE) {
		auto prevNode = createNode(prev);
		id->children.push_back(prevNode);
	}
}

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
		parseIdentifier(tokenList, toRet, pos);
	}

	return toRet;
}

std::vector<Node*> Parser::roots = {};

void Parser::parseTokens(TOKEN_LIST) {

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

	roots = branches;
}
