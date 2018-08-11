#include "Loader.h"
#include "Parser.h"
#include "ActionTree.h"
#include "Program.h"

#include <iostream>
#include <string>

#include <chrono>

#define TIMING 0

/*
Maybe add some documentation so other people can understand what each function does.
Great code btw, I see alot of the design patterns in this code that the enterprise lecture talked about, good stuff.
*/

#if TIMING
using millif = std::chrono::duration<float, std::milli>;
auto start = std::chrono::high_resolution_clock::now();
#endif

inline void setTimer() {
#if TIMING
	start = std::chrono::high_resolution_clock::now();
#endif
}


std::vector<std::string> loadedProgram;
Parser parseTree;
ActionTree actions;

bool loaded = false, parsed = false;

//Ensure a valid user inputted location
std::string getFileLocation() {

	std::string fileLoc;
	std::cout << "Enter a file to load: ";
	std::cin >> fileLoc;

	while(!isValidFile(fileLoc)) {
		std::cout << "Invalid file " << fileLoc << std::endl;
		std::cout << "Enter a different file location: ";
		std::cin >> fileLoc;
	}

	return fileLoc;
}

inline void loadProgram() {

	const auto progLoc = getFileLocation();

	setTimer();
	loadedProgram = fileContents(progLoc);

#if TIMING
	using namespace std::chrono;
	auto loadingTime = high_resolution_clock::now() - start;
	std::cout << "Loading program took " << duration_cast<millif>(loadingTime).count() << "ms" << std::endl;
	setTimer();
#endif

	loaded = true;
	parsed = false;
}

inline void parse() {
	if(!loaded) {
		loadProgram();
	}

	setTimer();

	const auto tokenList = tokens(loadedProgram);

#if TIMING
	using namespace std::chrono;
	auto tokeningTime = high_resolution_clock::now() - start;
	std::cout << "Tokenizing the program took " << duration_cast<millif>(tokeningTime).count() << "ms" << std::endl;
	setTimer();
#endif

	parseTree = Parser(tokenList);

#if TIMING
	using namespace std::chrono;
	auto parsingTime = high_resolution_clock::now() - start;
	std::cout << "Parsing the program took " << duration_cast<millif>(parsingTime).count() << "ms" << std::endl;
	setTimer();
#endif

	actions.writeActionTreeList(parseTree);

#if TIMING
	using namespace std::chrono;
	auto loadingTime = high_resolution_clock::now() - start;
	std::cout << "Writing the action trees took " << duration_cast<millif>(loadingTime).count() << "ms" << std::endl;
	setTimer();
#endif

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
			std::cout << "Main returned " << static_cast<int>(ret) << std::endl;
			break;
		case VariableType::STRING:
			std::cout << "Main returned " << static_cast<std::string>(ret) << std::endl;
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
		std::cout << "Nython v0.0.1b" << std::endl;
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
				std::cout << "Invalid option" << std::endl;
			case 0: break; //Jumps over everything
		}
		std::cout << std::endl;

	} while(option != 0);
	return option;
}
