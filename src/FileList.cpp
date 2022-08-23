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
<body style=\"background: black; color: #f7eedf; font-family: monospace;\">\
	<h1 style=\"margin: 2rem auto 1rem; width: 50%; font-size: 2.05rem;\">Index of ");
	res.append(route);
	res.append("</h1>");
	return res;
}

std::string getItemHtml(std::string& filename, std::string& path) {
	std::string item;
	std::string path_for_link = path == "/" ? "" : path;

	item.append("<li>");
	item.append("<a style=\"text-decoration: none; color: inherit;\" href=\"" + path_for_link  + "/" + filename + "\">" + filename);
	item.append("</li>");
	return item;
}

void	setHtml(std::string& html, std::vector<std::string>& list, std::string& root,
			std::string& route) {
	html.append(getPageHeader(root, route));
	html.append("<ul style=\"font-size: 1.55rem; color: #f7eedf;\">");

	for (size_t i = 0; i < list.size(); i++) {
		html.append(getItemHtml(list[i], route));
	}

	html.append("</ul>\n");
}


std::string  FileList::getFileListHTML(std::string root, std::string route) {
	std::string 				html;
	std::vector<std::string>	list;

	getList(root, list);
	setHtml(html, list, root, route);
	return html;
}
