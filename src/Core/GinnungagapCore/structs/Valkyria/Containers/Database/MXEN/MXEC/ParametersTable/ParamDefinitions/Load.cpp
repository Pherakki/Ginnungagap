#include "Load.hpp"
#include <filesystem>
#include <iostream>


void loadParameterDefinitions(ParameterDefMap& defmap, const std::filesystem::path& directory)
{
    loadParameterDefinitions(defmap, directory.string());
}

void loadParameterDefinitions(ParameterDefMap& defmap, const std::string& directory)
{
    for (const auto& dir_entry : std::filesystem::directory_iterator{directory})
    {
        const auto& fp = dir_entry.path();
        auto key       = fp.stem().string();
        auto jsondata  = loadJSON(fp.string());
        defmap.insert({key, MXEParameterSetGroupDefinition{key, jsondata}});
    }
}

void loadEntityDefinitions(EntityDefMap& defmap, const std::filesystem::path& directory)
{
    loadEntityDefinitions(defmap, directory.string());
}

void loadEntityDefinitions(EntityDefMap& defmap, const std::string& directory)
{
    for (const auto& dir_entry : std::filesystem::directory_iterator{directory})
    {
        const auto& fp = dir_entry.path();
        auto key       = fp.stem().string();
        auto jsondata  = loadJSON(fp.string());
        defmap.insert({key, MXEEntityDefinition{key, jsondata}});
    }
}


FlatEntityDefMap flattenEntityDefinitions(EntityDefMap& defmap)
{
    FlatEntityDefMap flat_defmap;
    for (const auto& [k,v] : defmap)
        flat_defmap.insert({k, MXEFlatEntityDefinition(v, defmap)});
    return flat_defmap;
}
