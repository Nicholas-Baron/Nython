#include "Loader.hpp"
#include "Parser.hpp"
#include "ActionTree.hpp"
#include "Program.hpp"

#include <iostream>
#include <string>

#include <chrono> //For timing

//Macros that can not be changed to constexpr
#define TIMING 0
#define SIZING 0

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

std::string programName;
std::vector<std::string> loadedProgram;
Parser parseTree;
ActionTree actions;

bool loaded = false, parsed = false, formedActions = false, devOptions = false;

//Ensure a valid user inputted location
std::string getFileLocation() {
	std::string fileLoc;
	std::cout << "Enter a file to load: ";
	std::cin >> fileLoc;

	while(!Loader::isValidFile(fileLoc)) {
		std::cout << "Invalid file " << fileLoc << std::endl;
		std::cout << "Enter a different file location: ";
		std::cin >> fileLoc;
	}

	return fileLoc;
}

void loadProgram() {
	programName = getFileLocation();

	setTimer();
	loadedProgram = Loader::fileContents(programName);

#if TIMING
	using namespace std::chrono;
	auto loadingTime = high_resolution_clock::now() - start;
	std::cout << "Loading program took " << duration_cast<millif>(loadingTime).count() << "ms" << std::endl;
	setTimer();
#endif

#if SIZING
	auto loadedSize = sizeof(loadedProgram);
	for(const auto& item : loadedProgram) { loadedSize += sizeof(item); }
	std::cout << "Size of Loaded Program: " << loadedSize << " bytes" << std::endl;
#endif

	loaded = true;
	parsed = false;
	formedActions = false;
}

void parse() {
	if(!loaded) {
		loadProgram();
	}

	setTimer();

	const auto tokenList = Loader::tokens(loadedProgram);

#if TIMING
	using namespace std::chrono;
	auto tokeningTime = high_resolution_clock::now() - start;
	std::cout << "Tokenizing the program took " << duration_cast<millif>(tokeningTime).count() << "ms" << std::endl;
	setTimer();
#endif

#if SIZING
	auto tokenSize = sizeof(tokenList);
	for(const auto& item : tokenList) { tokenSize += sizeof(item); }
	std::cout << "Size of Token List: " << tokenSize << " bytes" << std::endl;
#endif

	parseTree = Parser(tokenList);

#if TIMING
	using namespace std::chrono;
	auto parsingTime = high_resolution_clock::now() - start;
	std::cout << "Parsing the program took " << duration_cast<millif>(parsingTime).count() << "ms" << std::endl;
	setTimer();
#endif

#if SIZING
	std::cout << "Size of Parse Tree: " << parseTree.memFootprint() << " bytes" << std::endl;
#endif

	parsed = true;
	formedActions = false;
}

void formActionTrees() {
	if(!parsed) {
		parse();
	}

	actions.writeActionTreeList(parseTree);

#if TIMING
	using namespace std::chrono;
	auto loadingTime = high_resolution_clock::now() - start;
	std::cout << "Writing the action trees took " << duration_cast<millif>(loadingTime).count() << "ms" << std::endl;
	setTimer();
#endif

#if SIZING
	std::cout << "Size of Action: " << actions.memFootprint() << " bytes" << std::endl;
#endif

	formedActions = true;
}

void showTokens() {
	if(!loaded) {
		loadProgram();
	}

	std::cout << "Tokens for " << programName << std::endl;
	for(const auto& token : Loader::tokens(loadedProgram)) {
		std::cout << *token << std::endl;
	}
}

void showParseTrees() {
	if(!parsed) {
		parse();
	}

	std::cout << "Parse Trees for " << programName << std::endl;
	for(unsigned i = 0; i < parseTree.parsedTokens().size(); i++) {
		Parser::readNode(parseTree.parsedTokens()[i].get());
		std::cout << " Line #" << i << std::endl;
	}
	std::cout << std::endl;
}

void showActionTrees() {
	if(!formedActions) {
		formActionTrees();
	}

	std::cout << "Action Trees for " << programName << std::endl;
	for(unsigned i = 0; i < actions.actionList().size(); i++) {
		std::cout << "Action Set #" << i << std::endl;
		actions.printActionTree(actions.actionList().at(i));
	}
	std::cout << std::endl;
}

void runProgram() {
	if(!formedActions) {
		formActionTrees();
	}

	std::cout << "Running " << programName << std::endl;
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

void devOperations(const unsigned option) {
	switch(option) {
		case 11:
			showTokens(); break;
		case 12:
			showParseTrees(); break;
		case 13:
			showActionTrees(); break;
		default:
			std::cout << "Invalid option" << std::endl;
	}
}

int main() {
	unsigned option;

	do {
		//Main menu
		std::cout << "Nython v0.0.2a (Tools Update)" << std::endl;
		std::cout << "0. Exit" << std::endl;
		std::cout << "1. Reserved Word List" << std::endl;
		std::cout << "2. Load Program" << std::endl;
		std::cout << "3. Run Program" << std::endl;
		std::cout << "9. Enable Special Tools" << std::endl;

		if(devOptions) {
			std::cout << "11. Show Token List" << std::endl;
			std::cout << "12. Show Parser Output" << std::endl;
			std::cout << "13. Show Action Trees" << std::endl;
		}

		std::cout << "Select an option: ";
		std::cin >> option;
		std::cout << std::endl;

		if(devOptions && option > 10) {
			devOperations(option);
		} else {
			switch(option) {
				case 1:
					Keywords::printKeywords(); break;
				case 2:
					loadProgram(); break;
				case 3:
					runProgram(); break;
				case 9:
					unsigned code;
					std::cout << "Enter the special unlock code: ";
					std::cin >> code;
					devOptions = (code == 1999042) ? !devOptions : false;
					break;
				default:
					std::cout << "Invalid option" << std::endl;
				case 0: break; //Jumps over everything
			}
		}
		std::cout << std::endl;
	} while(option != 0);
	return option;
}
