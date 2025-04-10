#pragma once

#include <GinnungagapCore/structs/Valkyria/Filetypes/MXE/BinaryVC4.hpp>
#include <filesystem>

void unpackMXEToCSV(const MXE& mxe, const ParameterDefMap& params_defmap, const FlatEntityDefMap& entity_defmap, const std::string& path, bool terse=false);
void unpackParametersTable(const MXE& mxe, const ParameterDefMap&  params_defmap, std::filesystem::path& root_path, bool terse=false);
void unpackEntitiesTable  (const MXE& mxe, const FlatEntityDefMap& entity_defmap, std::filesystem::path& root_path);
void unpackPathsTable     (const MXE& mxe, std::filesystem::path& root_path);
void unpackAssetsTable    (const MXE& mxe, std::filesystem::path& root_path);
void unpackGroupsTable    (const MXE& mxe, std::filesystem::path& root_path);
