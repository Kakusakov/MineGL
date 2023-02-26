#include "EngineFileIO.hpp"
#include <sstream>
#include <fstream>

bool tryLoadTextFile(const std::string& path, std::string& output) {
	std::ifstream file(path);
	if (file.fail()) return false;
	std::stringstream buffer;
	buffer << file.rdbuf();
	output = buffer.str();
	return true;
}
