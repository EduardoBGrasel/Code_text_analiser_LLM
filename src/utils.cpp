#include "utils.hpp"

std::string Read(std::string filename) {

    std::fstream file(filename);
    if (!file.is_open()) return "Error: file not found";
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();

}

bool Write(std::string filename, std::string content) {
    std::ofstream file(filename);
    if (!file.is_open()) return -1;
    file << content;
    file.close();
    return 0;
}

std::string Bash_exec(std::string command) {
    // capture everything
    std::string full_command = command + " 2>&1";
    
    char buffer[128];
    std::string result = "";
    

    FILE* pipe = popen(full_command.c_str(), "r");
    if (!pipe) return "Erro ao abrir pipe!";

    try {
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    
    pclose(pipe);
    return result; 
}