#include "Loader.h"
#include "Parser.h"
#include "Program.h"

#include <iostream>
#include <string>

#define PRINT_PARSE_TREE 0
/*maybe add some documentation so other people(me included) can understand what each function does. Great code btw, I see alot of 
the design patters in this code that the enterprise lecture talked about good stuff.*/
std::string getFileLocation () {
	
	std::string fileLoc;
	std::cout << "Enter a file to load: ";
	std::cin >> fileLoc;
	
	//Ensure valid location
	while (!isValidFile (fileLoc)) {
		std::cout << "Invalid file " << fileLoc << std::endl;
		std::cout << "Enter a different file location: ";
		std::cin >> fileLoc;
	}

	return fileLoc;
}

void runInterpreter() {
	auto fileLoc = getFileLocation();
	auto content = fileContents(fileLoc);
	auto tokenList = tokens(content);
	auto parseTree = Parser(tokenList);
	if(PRINT_PARSE_TREE) {
		for(unsigned i = 0; i < parseTree.parsedTokens().size(); i++) {
			Parser::readNode(parseTree.parsedTokens()[i]);
			std::cout << " Line #" << i << std::endl;
		}
	}
	std::cout << std::endl;
	Program prog(parseTree);
	prog.run("main");
	std::cout << std::endl;
}

int main() {

	unsigned option;

	do {
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
