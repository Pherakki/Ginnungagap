#pragma once

#include <unordered_map>
#include "Definition.hpp"


void loadParameterDefinitions(ParameterDefMap& defmap, const std::string& directory);
void loadParameterDefinitions(ParameterDefMap& defmap, const std::filesystem::path& directory);
void loadEntityDefinitions(EntityDefMap& defmap, const std::string& directory);
void loadEntityDefinitions(EntityDefMap& defmap, const std::filesystem::path& directory);
FlatEntityDefMap flattenEntityDefinitions(EntityDefMap& defmap);
