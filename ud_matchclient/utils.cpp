#include "utils.h"

std::string load_file(std::string file) {
	std::ifstream f (file);
	std::stringstream fs;
	fs << f.rdbuf();
	return fs.str();
}
