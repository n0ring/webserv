#include "FileList.hpp"

void getList(std::string& path, std::vector<std::string>& list) {
	struct dirent *entry;
	DIR *dir = opendir(path.c_str());

	if (dir == NULL) {
		return ;
	}
	while ((entry = readdir(dir)) != NULL) {
		std::string current(entry->d_name);
		if (current == "." || current == ".." || current.empty()) {
			continue;
		}
		list.push_back(current);
	}
	closedir(dir);
}

std::string getPageHeader(std::string& path, std::string& route) {
	std::string res;

	res.append("\
	<!DOCTYPE html>\
<html>\
<head>\
	<meta charset='utf-8'>\
	<title>");
	res.append(path);
	res.append("</title>\
</head>\
<body style=\"background: black;\">\
	<h1 style=\"color: blueviolet;  margin: auto; width: 50%\">Index of ");
	res.append(route);
	res.append("</h1>");
	return res;
}

std::string getItemHtml(std::string& filename, std::string& path) {
	std::string item;
	(void)filename;
	item.append("<li>");
	item.append("<a href=\"" +filename + "\">" + filename);
	item.append("</li>");
	(void) path;
	return item;
}

void	setHtml(std::string& html, std::vector<std::string>& list, std::string& path,
			std::string& route) {
	for (size_t i = 0; i < list.size(); i++) {
		std::cout << list[i] << std::endl;
	}
	html.append(getPageHeader(path, route));
	html.append("<ul>");

	for (size_t i = 0; i < list.size(); i++) {
		html.append(getItemHtml(list[i], path));
	}

	html.append("</ul>\n");
	(void)path;
}


std::string  FileList::getFileListHTML(std::string dir, std::string route) {
	std::string 				html;
	std::vector<std::string>	list;

	getList(dir, list);
	setHtml(html, list, dir, route);
	std::cout << html << std::endl;
	return html;
}
