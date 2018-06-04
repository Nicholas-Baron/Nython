#include "Parser.h"

#include <unordered_map>

std::unordered_map < TokenType, std::vector<Token*>> separateByType (const std::vector<Token*>& list ) { 

	std::unordered_map<TokenType, std::vector<Token*>> toRet;

	for (unsigned i = 0; i < list.size(); i++) {
		toRet[list[i]->type].push_back (list[i]);
	}

	return toRet;
}

void Parser::parseTokens (const std::vector<Token*>& tokenList) { 

	auto seperated = separateByType (tokenList);

	std::vector<Node*> branchBundle;
	std::vector<int> locationsBundled;
	for (unsigned i = 0; i < seperated[TokenType::IDENTIFIER].size(); i++) {
		auto location = position (tokenList, seperated[TokenType::IDENTIFIER][i]);
		auto previous = tokenList[location - 1];

		if (previous->type == TokenType::TYPE) {
			Node* declType = new Node;
			declType->token = previous;
			
			Node* declaration = new Node;
			declaration->token = tokenList[location];
			
			linkNodes (declaration, declType);

			branchBundle.push_back (declaration);
			locationsBundled.push_back (location);
		} else if(previous->type == TokenType::COMMAND){
			Node* comm = new Node;
			comm->token = previous;

			Node* variable = new Node;
			variable->token = tokenList[location];

			linkNodes (comm, variable);

			branchBundle.push_back (comm);
			locationsBundled.push_back (location - 1);
		} else if (previous->type == TokenType::DELINEATOR) {
			Node* delin = new Node;
			delin->token = previous;

			Node* variable = new Node;
			variable->token = tokenList[location];

			linkNodes (delin, variable);

			branchBundle.push_back (delin);
			locationsBundled.push_back (location - 1);
		} else if (previous->type == TokenType::OPERATOR) {
			Node* oper = new Node;
			oper->token = previous;

			Node* variable = new Node;
			variable->token = tokenList[location];

			linkNodes (oper, variable);

			branchBundle.push_back (oper);
			locationsBundled.push_back (location - 1);
		}
	}

	for (unsigned i = 0; i < seperated[TokenType::OPERATOR].size ( ); i++) {
		auto location = position (tokenList, seperated[TokenType::OPERATOR][i]);
		auto previous = tokenList[location - 1];
		auto next = tokenList[location + 1];

		if (previous->type == TokenType::IDENTIFIER) {
			Node *left, *right = new Node, *op;
			bool hasRight = false;

			auto prevPos = position (locationsBundled, location - 1);
			if (prevPos >= 0) {
				left = branchBundle[prevPos];
			} else {
				left = new Node;
				left->token = previous;
			}

			if (next->type == TokenType::COMMAND || next->type == TokenType::IDENTIFIER || next->type==TokenType::LITERAL) {
				right->token = next;
				hasRight = true;
			}
			
			op = new Node;
			op->token = tokenList[location];

			linkNodes (op, left);
			if (hasRight) { linkNodes (op, right); } else { delete right; }
			branchBundle.push_back (op);
			locationsBundled.push_back (location);
		}
	}

	for (unsigned i = 0; i < seperated[TokenType::COMMAND].size(); i++) {
		auto location = position (tokenList, seperated[TokenType::OPERATOR][i]);
		auto next = tokenList[location + 1];

		if (next->type==TokenType::DELINEATOR) {

		}
	}

}
