#include <iostream>
#include <vector>

std::string getInput(void) {
	std::string tmp;
	for (std::string line; std::getline(std::cin, line);) {
		tmp.append(line);
	}
	return tmp;
}
void splitByChar(std::string &s, char delimiter, std::vector<std::string>& v) {
	size_t start = 0, end;
	while (start < s.length()) {
		while (start < s.length() && s[start] == delimiter ) start++;
		end = start;
		while (end < s.length() && s[end] != delimiter) end++;
		v.push_back(s.substr(start, end - start));
		start = end + 1;
	}
}

void	printSuccess(std::string inputName) {
std::cout << "Content-Type: text/html" << std::endl << std::endl;
std::cout << "<!DOCTYPE html> \n\
<html lang=\"en\"> \n\
<head> \n\
	<meta charset=\"UTF-8\"> \n\
	<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"> \n\
	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> \n\
	<link rel=\"preconnect\" href=\"https://fonts.googleapis.com\"> \n\
	<link rel=\"preconnect\" href=\"https://fonts.gstatic.com\" crossorigin> \n\
	<link href=\"https://fonts.googleapis.com/css2?family=Rubik+Wet+Paint&display=swap\" rel=\"stylesheet\"> \n\
	<link rel=\"stylesheet\" href=\"style/red-coast-style.css\" type=\"text/css\"> \n\
	<title>Red Coast</title> \n\
</head> \n\
<body> \n\
	<main class=\"red-coast-main\"> \n\
		<p class=\"red-coast-warning\"> \n\
			Foolish ";
			std::cout << inputName;
			std::cout << ", you are lucky that I am the one to receive your signal.<br> \n\
			I have mercy for you, pitiful creatures, thus I won't let anyone know about your message.<br> \n\
			Once they hear your voice, they will head towards your system to take over your home and make it theirs.<br> \n\
			DO NOT ANSWER!<br> \n\
			DO NOT ANSWER!<br> \n\
			DO NOT ANSWER! \n\
		</p> \n\
	</main> \n\
</body> \n\
</html>" << std::endl;
}


int main(void) {
	std::string					input;
	std::vector<std::string>	inputV;

	input = getInput();
	// std::cerr << input << std::endl;
	// splitByChar(input, '=', inputV);
	// if (inputV.size() != 2) {
	// 	printSuccess("unknown");
	// }
	// else {
		printSuccess(input);
	// }
	return 0;
}