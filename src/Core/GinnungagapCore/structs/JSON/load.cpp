#include "load.hpp"
#include <fstream>



nlohmann::ordered_json loadJSON(const std::string& filepath)
{
    std::ifstream f(filepath);
    return nlohmann::ordered_json::parse(f);
}

