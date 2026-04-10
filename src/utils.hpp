#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>

std::string Read(std::string filename);

bool Write(std::string filename, std::string content);

std::string Bash_exec(std::string command);