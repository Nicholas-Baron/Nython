#include "Loader.h"
#include "Parser.h"
#include "Program.h"

#include <iostream>
#include <string>

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
	Parser::parseTokens (tokenList);
	
	for (unsigned i = 0; i < Parser::parsedTokens().size(); i++) {
		Parser::readNode (Parser::parsedTokens ( )[i]);
		std::cout << " Line #"<< i << std::endl;
	}
	std::cout << std::endl;
	Program prog;
	prog.run("main");

	//Windows pausing
#if _WIN32
	system("pause");
#endif // _WIN32

	return 0;
}