#include "Loader.h"
#include "Parser.h"
#include "ActionTree.h"
//#include "Program.h"

#include <iostream>
#include <string>

/*
Maybe add some documentation so other people can understand what each function does. 
Great code btw, I see alot of the design patterns in this code that the enterprise lecture talked about, good stuff.
*/
#define PRINT_TOKEN_LIST 0
#define PRINT_PARSE_TREE 0
#define PRINT_ACTION_TREE 1
#define RUN_INTERPRETER 0

//Ensure a valid user inputted location
std::string getFileLocation () {
	
	std::string fileLoc;
	std::cout << "Enter a file to load: ";
	std::cin >> fileLoc;
	
	while (!isValidFile (fileLoc)) {
		std::cout << "Invalid file " << fileLoc << std::endl;
		std::cout << "Enter a different file location: ";
		std::cin >> fileLoc;
	}

	return fileLoc;
}

//Gets the file from the user
void runInterpreter() {
	auto fileLoc = getFileLocation();
	auto content = fileContents(fileLoc);
	auto tokenList = tokens(content);
	
#if PRINT_TOKEN_LIST
	for(unsigned i = 0; i < tokenList.size(); i++) {
		std::cout << *tokenList[i] << std::endl;
	}
	std::cout << std::endl;
#endif

	auto parseTree = Parser(tokenList);

#if PRINT_PARSE_TREE
	for(unsigned i = 0; i < parseTree.parsedTokens().size(); i++) {
		Parser::readNode(parseTree.parsedTokens()[i]);
		std::cout << " Line #" << i << std::endl;
	}
	std::cout << std::endl;
#endif
	
	ActionTree actions;
	actions.writeActionTree(parseTree.parsedTokens());

#if PRINT_ACTION_TREE
	for(unsigned i = 0; i < actions.actionList().size(); i++) {
		if(actions.actionList()[i] != nullptr) actions.printActionTree(actions.actionList()[i]);
		std::cout << " Action Set #" << i << std::endl;
	}
	std::cout << std::endl;
#endif

#if RUN_INTERPRETER
	Program prog(parseTree);
	auto ret = prog.run("main");
	switch(ret.type) {
		case INT:
			std::cout << "Main returned " << *(static_cast<int*>(ret.data)) << std::endl;
			break;
		case STRING:
			std::cout << "Main returned " << *(static_cast<std::string*>(ret.data)) << std::endl;
			break;
		case VOID:
		default:
			break;
	}

	std::cout << std::endl;
#endif
}

int main() {

	unsigned option;

	do {
		//Main menu
		std::cout << "Nython v1.0a" << std::endl;
		std::cout << "0. Exit" << std::endl;
		std::cout << "1. Reserved Word List" << std::endl;
		std::cout << "2. Run Interpreter" << std::endl;
		std::cout << "3. [Construction Here]" << std::endl;
		
		std::cout << "Select an option: ";
		std::cin >> option;
		std::cout << std::endl;

		switch(option) {
			case 1:
				Keywords::printKeywords(); break;
			case 2:
				runInterpreter(); break;
			default:
				std::cout << "Invalid option" << std::endl; break;
			case 0: break; //Jumps over everything
		}
		std::cout << std::endl;
	
	} while(option != 0);
	return 0;
}
