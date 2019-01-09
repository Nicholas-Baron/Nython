#include "Loader.hpp"
#include "Parser.hpp"
#include "ActionTree.hpp"
#include "Program.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <unordered_map>

/*
Maybe add some documentation so other people can understand what each function does.
Great code btw, I see alot of the design patterns in this code that the enterprise lecture talked about, good stuff.
*/

//This struct holds all data we know about a loaded program
struct LoadedProgram {
	std::string name;
	Parser parseTree;
	ActionTree actions;
	std::vector<std::string> source;

	LoadedProgram(const std::string& name_in) : name(name_in) {
		source = Loader::fileContents(name);
		parseTree = Parser(Loader::tokens(source));
		actions.writeActionTreeList(parseTree);
	}

	size_t mem_footprint() const {
		size_t toRet = sizeof(*this);
		for(const auto& line : source) {
			toRet += sizeof(line);
		}
		return toRet + parseTree.memFootprint() + actions.memFootprint();
	}
};

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

//Loads a program from the user
inline LoadedProgram loadProgram() {
	return LoadedProgram(getFileLocation());
}

void showTokens(const LoadedProgram& program) {
	std::cout << "Tokens for " << program.name << std::endl;
	for(const auto& token : Loader::tokens(program.source)) {
		std::cout << *token << std::endl;
	}
}

void showParseTrees(const LoadedProgram& program) {
	std::cout << "Parse Trees for " << program.name << std::endl;
	size_t i = 0;
	std::for_each(program.parseTree.parsedTokens().begin(),
				  program.parseTree.parsedTokens().end(),
				  [&](const std::shared_ptr<Node>& val) -> void {
		Parser::readNode(val.get());
		std::cout << " Line #" << i++ << std::endl;
	}
	);
	std::cout << std::endl;
}

void showActionTrees(const LoadedProgram& program) {
	std::cout << "Action Trees for " << program.name << std::endl;
	size_t num = 0;
	std::for_each(program.actions.actionList().begin(),
				  program.actions.actionList().end(),
				  [&](auto& action) -> void {
		std::cout << "Action Set #" << num << std::endl;
		program.actions.printActionTree(action.get());
	});

	std::cout << std::endl;
}

void runProgram(const LoadedProgram& program) {
	std::cout << "Running " << program.name << std::endl;

	Program prog(program.actions);
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

int main(int arg_count, const char * const * const args) {
	//If an argument is given, run the program with a name of that argument
	if(arg_count > 1) {
		assert(arg_count == 2);
#pragma warning(suppress: 26481) //Stops Microsoft from asking about span
		LoadedProgram program(args[1]);
		runProgram(program);
		return 0; // MAYBE: load multiple programs and run sequentially
	}

	std::unordered_map<std::string, LoadedProgram> ready_programs;
	std::string current_program;

	bool devOptions = false;
	unsigned option = 0;

	do {
		//Main menu
		std::cout << "Nython v0.0.3a (Caching Update)" << std::endl;
		std::cout << "0. Exit" << std::endl;
		std::cout << "1. Reserved Word List" << std::endl;
		std::cout << "2. Load Program" << std::endl;
		std::cout << "3. Run Program" << std::endl;
		std::cout << "9. Enable Special Tools" << std::endl;

		if(devOptions) {
			std::cout << "11. Show Token List" << std::endl;
			std::cout << "12. Show Parser Output" << std::endl;
			std::cout << "13. Show Action Trees" << std::endl;
			std::cout << "14. Show Memory Footprint" << std::endl;
		}

		std::cout << "Select an option: ";
		std::cin >> option;
		std::cout << std::endl;

		if(devOptions && option > 10) {
			switch(option) {
				case 11:
					showTokens(ready_programs.at(current_program)); break;
				case 12:
					showParseTrees(ready_programs.at(current_program)); break;
				case 13:
					showActionTrees(ready_programs.at(current_program)); break;
				case 14: [&] { // An immediatly ran lambda
					using namespace std;
					cout << "Current Memory Footprint in Bytes" << endl;

					size_t total = sizeof(ready_programs);
					for_each(ready_programs.begin(), ready_programs.end(),
							 [&](auto& pair) -> void {
						cout << pair.first << " : "
							<< pair.second.mem_footprint() << endl;
						total += pair.second.mem_footprint();
					});

					cout << "Total: " << total << " bytes" << endl;
				}(); break;
				default:
					std::cout << "Invalid option" << std::endl;
			}
		} else {
			switch(option) {
				case 1:
					Keywords::printKeywords(); break;
				case 2:
					current_program = getFileLocation();
					ready_programs.insert(std::make_pair(current_program, LoadedProgram(current_program)));
					break;
				case 3:
					runProgram(ready_programs.at(current_program)); break;
				case 9: {
					using std::cin, std::cout;
					unsigned code = 0;
					cout << "Enter the special unlock code: ";
					cin >> code;
					//If the responce is correct, flip the switch.
					//Else disalbe all developer options
					devOptions = (code == 1999042) ? !devOptions : false;
				}
						break;
				default:
					std::cout << "Invalid option" << std::endl;
				case 0: break; //Jumps over everything to get to the exit
			}
		}
		std::cout << std::endl;
	} while(option != 0);
}
