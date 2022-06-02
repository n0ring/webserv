#pragma once

#include <poll.h>
#include <iostream>
#include <vector>
#include <fstream>

pollfd make_fd(int fd, int event); // inline? 


void convertFileToString(std::string &configName, std::string &config);
void deleteComments(std::string &config);
void removeSemicolon(std::string& s, std::string& line);
std::vector<std::string> sPPlit(std::string s);
std::string getLine(std::string& str, size_t& startPos);