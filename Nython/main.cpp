#include "Loader.h"
#include "Parser.h"
#include "ActionTree.h"
#include "Program.h"

#include <iostream>
#include <string>

/*
Maybe add some documentation so other people can understand what each function does. 
Great code btw, I see alot of the design patterns in this code that the enterprise lecture talked about, good stuff.
*/

std::vector<std::string> loadedProgram;
Parser parseTree;
ActionTree actions;

bool loaded = false, parsed = false;

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

inline void loadProgram() {
	//Loads the file from a user-inputted location
	loadedProgram = fileContents(getFileLocation());
	loaded = true;
	parsed = false;
}

inline void parse() { 
	if(!loaded) {
		loadProgram();
	}

	parseTree = Parser(tokens(loadedProgram)); 
	actions.writeActionTreeList(parseTree);
	parsed = true;
}

void showTokens() {
	
	if(!loaded) {
		loadProgram();
	}

	for(const auto token : tokens(loadedProgram)) {
		std::cout << *token << std::endl;
	}
}

void showParseTrees() {
	
	if(!parsed) {
		parse();
	}
	
	for(unsigned i = 0; i < parseTree.parsedTokens().size(); i++) {
		Parser::readNode(parseTree.parsedTokens()[i]);
		std::cout << " Line #" << i << std::endl;
	}
	std::cout << std::endl;
}

void showActionTrees() {
	
	if(!parsed) {
		parse();
	}
	
	for(unsigned i = 0; i < actions.actionList().size(); i++) {
		std::cout << "Action Set #" << i << std::endl;
		actions.printActionTree(actions.actionList()[i]);
	}
	std::cout << std::endl;
}

void runProgram() {

	if(!parsed) {
		parse();
	}
	
	std::cout << std::endl;
	Program prog(actions);
	auto ret = prog.run("main");
	std::cout << std::endl;
	switch(ret.type) {
		case VariableType::INT:
			std::cout << "Main returned " << *(static_cast<int*>(ret.location)) << std::endl;
			break;
		case VariableType::STRING:
			std::cout << "Main returned " << *(static_cast<std::string*>(ret.location)) << std::endl;
			break;
		case VariableType::VOID:
		default:
			break;
	}
}

int main() {

	unsigned option;

	do {
		//Main menu
		std::cout << "Nython v0.0.1a" << std::endl;
		std::cout << "0. Exit" << std::endl;
		std::cout << "1. Reserved Word List" << std::endl;
		std::cout << "2. Load Program" << std::endl; 
		std::cout << "3. Show Token List" << std::endl;
		std::cout << "4. Show Parser Output" << std::endl;
		std::cout << "5. Show Action Trees" << std::endl;
		std::cout << "6. Run Program" << std::endl;
		std::cout << "7. [Construction Here]" << std::endl;
		
		std::cout << "Select an option: ";
		std::cin >> option;
		std::cout << std::endl;

		switch(option) {
			case 1:
				Keywords::printKeywords(); break;
			case 2:
				loadProgram(); break;
			case 3:
				showTokens(); break;
			case 4:
				showParseTrees(); break;
			case 5:
				showActionTrees(); break;
			case 6: 
				runProgram(); break;
			default:
				std::cout << "Invalid option" << std::endl; break;
			case 0: break; //Jumps over everything
		}
		std::cout << std::endl;
	
	} while(option != 0);
	return option;
}
