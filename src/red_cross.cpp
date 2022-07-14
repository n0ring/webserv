#include <iostream>

std::string getInput(void) {
	std::string tmp;
	for (std::string line; std::getline(std::cin, line);) {
		tmp.append(line);
	}
	return tmp;
}

void	printError() {

}

void	printSuccess() {
	
}

int main(void) {
	std::string input;

	input = getInput();
	if (input.empty()) {
		printError();
	}
	else {
		printSuccess();
	}
	return 0;
}