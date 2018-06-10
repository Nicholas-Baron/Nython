#include "Loader.h"
#include "Parser.h"
#include "Program.h"

#include <iostream>
#include <string>

#define PRINT_PARSE_TREE 0

std::string getFileLocation (int argCount, char* args[] ) {
	std::string fileLoc;
	if (argCount >= 2) {
		fileLoc = args[1];
	} else {
		std::cout << "Enter a file to load: ";
		std::cin >> fileLoc;
	}

	//Ensure valid location
	while (!isValidFile (fileLoc)) {
		std::cout << "Invalid file " << fileLoc << std::endl;
		std::cout << "Enter a different file location: ";
		std::cin >> fileLoc;
	}

	return fileLoc;
}

int main(int argCount, char* args[]) {

	auto fileLoc = getFileLocation (argCount, args);
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

	//Windows pausing
#if _WIN32
	system("pause");
#endif // _WIN32

	return 0;
}