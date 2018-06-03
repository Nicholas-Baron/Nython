#include "Loader.h"

#include <iostream>
#include <string>


int main(int argCount, char* args[]) {

	std::string fileLoc;
	if(argCount >= 2) {
		fileLoc = args[1];
	} else {
		std::cout << "Enter a file to load: ";
		std::cin >> fileLoc;
	}

	//Ensure valid location
	while(!isValidFile(fileLoc)) {
		std::cout << "Invalid file " << fileLoc << std::endl;
		std::cout << "Enter a different file location: ";
		std::cin >> fileLoc;
	}

	auto content = fileContents(fileLoc);
	auto tokenList = tokens(content);

	for(unsigned i = 0; i < tokenList.size(); i++) {
		std::cout << "[" << tokenList[i].line_number << "] " << " " << tokenList[i].token << std::endl;
	}

#if _WIN32
	//Windows pausing
	system("pause");
#endif // _WIN32

	return 0;
}