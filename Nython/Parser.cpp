#include "Parser.hpp"

#include <algorithm>
#include <cassert>
#include <memory>

inline std::shared_ptr<Node> createNode(const std::shared_ptr<const Token>& t) {
	return std::make_shared<Node>(t);
}

using TOKEN_LIST = const std::vector<std::shared_ptr<const Token>>&;

unsigned nextOfType(TOKEN_LIST tokenList, unsigned start, TokenType type, bool walkBackwards = false) {
	unsigned toRet = start;
	int depth = 0;
	if(walkBackwards) {
		toRet--;
		while((tokenList.at(toRet)->type != type || depth != 0) && toRet > 0) {
			if(Keywords::isSecondOfPairDelin(*tokenList.at(toRet))) {
				depth++;
			} else if(Keywords::isFirstOfPairDelin(*tokenList.at(toRet))) { depth--; }

			toRet--;
		}
	} else {
		toRet++;
		while((tokenList.at(toRet)->type != type || depth != 0) && toRet < tokenList.size() - 1) {
			if(Keywords::isSecondOfPairDelin(*tokenList.at(toRet))) {
				depth--;
			} else if(Keywords::isFirstOfPairDelin(*tokenList.at(toRet))) { depth++; }

			toRet++;
		}
	}
	return toRet;
}

void Parser::readNode(Node* root, int depth) {
	if(root != nullptr) {
		std::cout << "[" << depth << "]" << *(root->token) << " ";

		if(root->children.size() >= 1) {
			std::for_each(root->children.begin(), root->children.end(), 
				[depth](auto iter){
					readNode(iter.get(), depth + 1);
				});
		}
	}
}

//Finds the closing parenthesis
auto otherEndOfPair(TOKEN_LIST tokenList, unsigned start, unsigned& end, bool walkBackwards = false) {
	std::shared_ptr<const Token> toRet;
	end = start;
	unsigned depth = 0;

	if(walkBackwards) {
		end--;
	} else {
		end++;
	}

	while(toRet == NULL) {
		if(Keywords::isSecondOfPairDelin(*tokenList.at(end)) && depth == 0) {
			toRet = tokenList.at(end);
		} else {
			if(Keywords::isFirstOfPairDelin(*tokenList.at(end))) {
				depth++;
			} else if(Keywords::isSecondOfPairDelin(*tokenList.at(end)) && depth != 0) {
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
auto endOfLoop(TOKEN_LIST tokenList, unsigned start, unsigned& end) {
	std::shared_ptr<const Token> toRet;
	end = start + 1;

	while(toRet == NULL) {
		if(Keywords::isEndOfLoop(tokenList.at(end).get())) {
			toRet = tokenList.at(end);
		} else { end++; }
	}

	return toRet;
}

//Finds the end of an if statement
auto endOfIf(TOKEN_LIST tokenList, unsigned start, unsigned& end) {
	const Token* toRet = nullptr;
	end = start + 1;

	while(toRet == nullptr) {
		if(Keywords::isEndOfConditional(*tokenList.at(end))) {
			toRet = tokenList.at(end).get();
		} else { end++; }
	}

	return toRet;
}

#undef TOKEN_LIST

void Parser::parseOperator(Node* addTo, unsigned& pos) {
	const bool isBin = Keywords::isBinaryOp(*tokenList.at(pos));
	auto leftLoc = nextOfType(tokenList, pos, TokenType::IDENTIFIER, true);

	auto prevNode = !Keywords::isAssignment(addTo->token.get()) && tokenList.at(pos - 1)->type == TokenType::DELINEATOR
		? parseToken(tokenList.at(leftLoc), leftLoc, false)
		: createNode(tokenList.at(leftLoc));

	if(isBin) {
		auto nextOp = nextOfType(tokenList, pos, TokenType::OPERATOR);
		bool nextOpUsed = false;

		std::shared_ptr<Node> nextNode;
		if(nextOfType(tokenList, pos, TokenType::TYPE) > nextOp && nextOfType(tokenList, pos, TokenType::COMMAND) > nextOp
		   && tokenList.at(pos + 2)->type == TokenType::OPERATOR) {
			nextNode = createNode(tokenList.at(nextOp));
			parseOperator(nextNode.get(), nextOp);
			nextOpUsed = true;
		} else {
			nextNode = createNode(tokenList.at(pos + 1));
		}
		assert(nextNode != nullptr);
		//nextNode is definitly set here

		//The type name is two spaces behind the current location
		if(tokenList.at(pos - 2)->type == TokenType::TYPE && Keywords::isAssignment(addTo->token.get())) {
			prevNode->children.push_back(createNode(tokenList.at(pos - 2)));
		}

		if(Keywords::isFirstOfPairDelin(*tokenList.at(pos + 2))) {
			pos++;
			parseIdentifier(nextNode.get(), pos, tokenList.at(pos + 1).get());
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
void Parser::parseAfterDelineator(Node* delin, unsigned pos) {
	unsigned finish = pos + 1;
	while(tokenList.at(finish)->type != TokenType::DELINEATOR) {
		finish++;
	}

	for(unsigned i = pos + 1; i < finish; i++) {
		auto t = tokenList.at(i), next = tokenList.at(i + 1);

		if(t->type == TokenType::OPERATOR) {
			auto op = createNode(t);
			parseOperator(op.get(), i);
			delin->children.push_back(op);
		} else if(next->type == TokenType::OPERATOR) {
			i++;
			auto op = createNode(next);
			parseOperator(op.get(), i);
			delin->children.push_back(op);
		} else if(t->type == TokenType::IDENTIFIER) {
			auto id = createNode(t);
			parseIdentifier(id.get(), i, tokenList.at(i + 1).get());
			delin->children.push_back(id);
		} else if(t->type == TokenType::LITERAL) {
			auto id = createNode(t);
			delin->children.push_back(id);
		}
	}
}

void Parser::parseCommand(Node* comm, unsigned& pos, bool isLast) {
	
	if(!isLast && tokenList.at(pos + 1) != nullptr 
	&& tokenList.at(pos + 1)->type == TokenType::DELINEATOR 
	&& Keywords::isFirstOfPairDelin(*tokenList.at(pos + 1))) {
		unsigned endOfPairPos = pos + 2;
		auto otherEnd = otherEndOfPair(tokenList, pos + 1, endOfPairPos);

		int depth = 0;
		for(unsigned j = pos + 1; j < endOfPairPos; j++) {
			const auto current = tokenList.at(j);

			if((current->type == TokenType::DELINEATOR && depth == 0) || Keywords::nonPairedDelin(*current)) {
				const auto delin = createNode(current);
				parseAfterDelineator(delin.get(), j);
				comm->children.push_back(delin);
			}

			if(Keywords::isFirstOfPairDelin(*current)) {
				depth++;
			} else if(Keywords::isSecondOfPairDelin(*current)) {
				depth--;
			}
		}

		auto delinEnd = createNode(otherEnd);
		comm->children.push_back(delinEnd);

		pos = endOfPairPos;

		if(Keywords::isLoopStart(comm->token.get())) {
			unsigned endLoopPos = pos;
			auto endLoop = endOfLoop(tokenList, pos, endLoopPos);
			pos++;
			parseSequence(delinEnd.get(), pos, endLoopPos, true);

			auto loopEnd = createNode(endLoop);
			delinEnd->children.push_back(loopEnd);

			pos = endLoopPos;
		} else if(Keywords::isConditionalStart(comm->token.get())) {
			unsigned endIfPos = pos;
			endOfIf(tokenList, pos, endIfPos);
			pos++;
			parseSequence(delinEnd.get(), pos, endIfPos, true);

			pos = endIfPos - 1;
		}
	} else {
		
		if(Keywords::needsParameter(comm->token.get())) {
			
			auto& next = tokenList.at(pos + 1);
			assert(next != nullptr);
			pos++;
			
			auto local_next = std::make_shared<const Token>(*next);
			auto param = parseToken(local_next, pos, pos + 1 == tokenList.size());
			comm->children.push_back(param);
		} else if(Keywords::isFuncEnd(comm->token.get())) {
			
			try{
				auto& next = tokenList.at(pos + 1);
			
				if(isLast || next == nullptr) {
					//A blank return (ends a void function)
					return;
				} else if(next->type == TokenType::LITERAL) {
					auto param = createNode(next);
					comm->children.push_back(param);
					pos++;
				} else if(next->type == TokenType::IDENTIFIER) {
					pos++;
					const auto nextCmd = nextOfType(tokenList, pos, TokenType::COMMAND);
					const auto nextType = nextOfType(tokenList, pos, TokenType::TYPE);

					const unsigned stop = std::min(nextCmd, nextType);
					parseSequence(comm, pos, stop, false);
					pos = stop;
				}
			}catch(...){
				return;
			}
		}
	}
}

//Reads a sequence of tokens for loops, if-statements, or long returns
void Parser::parseSequence(Node* addTo, unsigned start, unsigned end, bool multiLine) {
	auto nextOp = nextOfType(tokenList, start, TokenType::OPERATOR);

	if(nextOp < end && !multiLine) {
		nextOp--; //Moves it one back
		auto op = parseToken(tokenList.at(nextOp), nextOp, nextOp + 1 == end);
		addTo->children.push_back(op);
	} else {
		for(unsigned i = start; i < end; i++) {
			auto parsed = parseToken(tokenList.at(i), i, i + 1 == end);
			if(parsed != nullptr) {
				addTo->children.push_back(parsed);
			}
		}
	}
}

//Identifier is either a variable name or method name
void Parser::parseIdentifier(Node* id, unsigned& pos, const Token* next) {
	
	assert(next != nullptr);
	assert(id != nullptr);

	auto& prev = tokenList.at(pos - 1);

	if(prev->type == TokenType::TYPE) {
		id->children.push_back(createNode(prev));
	}

	if(next->type == TokenType::DELINEATOR && Keywords::isFirstOfPairDelin(*next)) {
		//This is a function call
		
		unsigned endOfPair = pos + 2;
		pos++;
		otherEndOfPair(tokenList, pos, endOfPair);

		auto nextDelin = nextOfType(tokenList, pos, TokenType::DELINEATOR);
		while(nextDelin <= endOfPair) {
			std::shared_ptr<Node> delin = createNode(tokenList.at(pos));
			parseAfterDelineator(delin.get(), pos);
			id->children.push_back(delin);
			pos = nextDelin;
			nextDelin = nextOfType(tokenList, pos, TokenType::DELINEATOR);
		}

		pos = endOfPair;
	}
}

//Parses a single token
std::shared_ptr<Node> Parser::parseToken(const std::shared_ptr<const Token>& t, unsigned& pos, bool isLast) {
	if(t == nullptr) { return nullptr; }

	assert(*tokenList.at(pos) == *t);

	std::shared_ptr<Node> toRet;
	
	if(!isLast && tokenList.at(pos + 1)->type == TokenType::OPERATOR) {
		toRet = createNode(tokenList.at(pos + 1));
		pos++;
		parseOperator(toRet.get(), pos);
		return toRet;
	}
	
	switch(t->type) {
		case TokenType::LITERAL:
			return createNode(t);
		case TokenType::IDENTIFIER:
			toRet = createNode(t);
			parseIdentifier(toRet.get(), pos, tokenList.at(pos + 1).get());
			return toRet;
		case TokenType::COMMAND:
			toRet = createNode(t);
			parseCommand(toRet.get(), pos, isLast);
			return toRet;	   
		default:
			break;
	}

	return nullptr;
}

void Parser::trimRoots() {
	std::vector<std::shared_ptr<Node>> temp;
	
	/*std::copy_if(roots.begin(), roots.end(), temp.begin(), 
				  [ ](const std::shared_ptr<Node>& iter) noexcept -> bool { return iter != nullptr; }
	);	*/

	for(const auto& iter : roots){
		if(iter != nullptr){
			temp.push_back(iter);
		}
	}

	roots = temp;
}

Parser::Parser(const std::vector<std::shared_ptr<const Token>>& tokenList_in)
	: tokenList(tokenList_in) {
	roots = parseTokens();
	trimRoots();
	roots.shrink_to_fit();
	ready = true;
}

std::vector<std::shared_ptr<Node>> Parser::parseTokens() {
	
	std::vector<std::shared_ptr<Node>> branches;
	
	for(size_t i = 0; i < tokenList.size(); i++) {
		const bool isLast = i + 1 == tokenList.size();
		branches.push_back(parseToken(tokenList.at(i), i, isLast));
	}
	
	return branches;
}
