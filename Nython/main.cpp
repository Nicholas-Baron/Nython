#include "Loader.hpp"
#include "Parser.hpp"
#include "ActionTree.hpp"
#include "Program.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>

/*
Maybe add some documentation so other people can understand what each function does.
Great code btw, I see alot of the design patterns in this code that the enterprise lecture talked about, good stuff.
*/

Parser parseTree;
ActionTree actions;

bool loaded = false, parsed = false, formedActions = false, 
devOptions = false, showSize = false;

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

std::vector<std::string> loadProgram(const std::string& programName) {
	
	std::vector<std::string> loadedProgram = Loader::fileContents(programName);

if(showSize){
	auto loadedSize = sizeof(loadedProgram);
	for(const auto& item : loadedProgram) { loadedSize += sizeof(item); }
	std::cout << "Size of Loaded Program: " << loadedSize << " bytes" << std::endl;
}

	loaded = true;
	parsed = false;
	formedActions = false;

	return loadedProgram;
}

void parse(std::string& programName, std::vector<std::string>& loadedProgram) {
	
	if(!loaded) {
		if(programName.empty()) { programName = getFileLocation(); }
		loadedProgram = loadProgram(programName);
	}

	const auto tokenList = Loader::tokens(loadedProgram);

if(showSize){
	auto tokenSize = sizeof(tokenList);
	for(const auto& item : tokenList) { tokenSize += sizeof(item); }
	std::cout << "Size of Token List: " << tokenSize << " bytes" << std::endl;
}

	parseTree = Parser(tokenList);

	if(showSize) {
	std::cout << "Size of Parse Tree: " << parseTree.memFootprint() << " bytes" << std::endl;
}

	parsed = true;
	formedActions = false;
}

void formActionTrees(std::string& programName, std::vector<std::string>& loadedProgram) {
	if(!parsed) {
		parse(programName, loadedProgram);
	}

	actions.writeActionTreeList(parseTree);

	if(showSize) {
	std::cout << "Size of Action: " << actions.memFootprint() << " bytes" << std::endl;
}

	formedActions = true;
}

void showTokens(const std::vector<std::string>& loadedProgram) {
	
	if(!loaded) {
		throw std::logic_error("Did not load the program");
	}

	//std::cout << "Tokens for " << programName << std::endl;
	for(const auto& token : Loader::tokens(loadedProgram)) {
		std::cout << *token << std::endl;
	}
}

void showParseTrees(std::string& programName, std::vector<std::string>& loadedProgram) {
	
	if(!parsed) {
		parse(programName, loadedProgram);
	}

	std::cout << "Parse Trees for " << programName << std::endl;
	size_t i = 0;
	std::for_each(parseTree.parsedTokens().begin(), parseTree.parsedTokens().end(), 
				  [&](const std::shared_ptr<Node>& val) -> void {
		              Parser::readNode(val.get());
		              std::cout << " Line #" << i++ << std::endl;
	              }
	);
	std::cout << std::endl;
}

void showActionTrees(std::string& programName, std::vector<std::string>& loadedProgram) {
	
	if(!formedActions) {
		formActionTrees(programName, loadedProgram);
	}

	//std::cout << "Action Trees for " << programName << std::endl;
	for(unsigned i = 0; i < actions.actionList().size(); i++) {
		std::cout << "Action Set #" << i << std::endl;
		actions.printActionTree(actions.actionList().at(i).get());
	}
	std::cout << std::endl;
}

void runProgram(std::string& programName, std::vector<std::string>& loadedProgram) {
	
	if(!formedActions) {
		formActionTrees(programName, loadedProgram);
	}

	std::cout << "Running " << programName << std::endl;
	
	Program prog(actions);
	const auto ret = prog.run("main");
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

//TODO: struct LoadedProgram{}

int main(int arg_count, const char * const * const args) {
	
	std::string programName;
	std::vector<std::string> loadedProgram;
	
	//If an argument is given, run the program with a name of that argument
	if(arg_count > 1){
		assert(arg_count == 2);
		programName = args[1];
		loadedProgram = loadProgram(programName);
		runProgram(programName, loadedProgram);
		return 0;
	}

	unsigned option = 0;

	do {
		//Main menu
		std::cout << "Nython v0.0.3a" << std::endl;
		std::cout << "0. Exit" << std::endl;
		std::cout << "1. Reserved Word List" << std::endl;
		std::cout << "2. Load Program" << std::endl;
		std::cout << "3. Run Program" << std::endl;
		std::cout << "9. Enable Special Tools" << std::endl;

		if(devOptions) {
			std::cout << "11. Show Token List" << std::endl;
			std::cout << "12. Show Parser Output" << std::endl;
			std::cout << "13. Show Action Trees" << std::endl;
			std::cout << "14. Toggle Show Memory Footprint" << std::endl;
		}

		std::cout << "Select an option: ";
		std::cin >> option;
		std::cout << std::endl;

		if(devOptions && option > 10) {
			switch(option) {
				case 11:
					showTokens(loadedProgram); break;
				case 12:
					showParseTrees(programName, loadedProgram); break;
				case 13:
					showActionTrees(programName, loadedProgram); break;
				case 14:
					showSize = !showSize; break;
				default:
					std::cout << "Invalid option" << std::endl;
			}
		} else {
			switch(option) {
				case 1:
					Keywords::printKeywords(); break;
				case 2:
					programName = getFileLocation();
					loadedProgram = loadProgram(programName); break;
				case 3:
					runProgram(programName, loadedProgram); break;
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
	
}
