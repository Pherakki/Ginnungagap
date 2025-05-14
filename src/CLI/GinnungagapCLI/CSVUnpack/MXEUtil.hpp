#pragma once
#include "GinnungagapCore/structs/Valkyria/Containers/Database/MXEN/MXEC/ParametersTable/ParamDefinitions/Load.hpp"

void addEntityToCSVUnpack(const std::filesystem::path& root_path, const std::string& entity_name, const FlatEntityDefMap& entity_defmap);

void addParamToCSVUnpack(const std::filesystem::path& param_csv, uint32_t param_id);
